#include "ruby.h"

#include <usb.h>
#include "ptp.h"
#include "ptpcam.h"

static VALUE rb_cPTP;
int num_devices;

#define D_MODEL "model"
#define D_BUS "bus"
#define D_DEV "dev"
#define D_FILES "@files"
#define D_DEVICES "@devices"

VALUE stoi(VALUE input) {
  return rb_funcall(input, rb_intern("to_i"), 0);
}

VALUE t_scan(VALUE self) {
  VALUE devices;
  PTPParams params;
  PTP_USB ptp_usb;
  PTPDeviceInfo deviceinfo;
  
  struct usb_bus *bus;
  struct usb_device *dev;
  
  devices = rb_ary_new();
  bus = init_usb();
  num_devices = 0;
  
  for (; bus; bus = bus->next) {
    for (dev = bus->devices; dev; dev = dev->next) {
      if (dev->config) {
        if (dev->config->interface->altsetting->bInterfaceClass == USB_CLASS_PTP) {;
          if (dev->descriptor.bDeviceClass != USB_CLASS_HUB) {
            VALUE item_hash;
            
            find_endpoints(dev, &ptp_usb.inep, &ptp_usb.outep, &ptp_usb.intep);
            init_ptp_usb(&params, &ptp_usb, dev);
            CR(ptp_opensession (&params,1), "Could not open session! Try to reset the camera.\n");
            CR(ptp_getdeviceinfo (&params, &deviceinfo), "Could not get device info!\n");
            
            item_hash = rb_hash_new();
            
            rb_hash_aset(item_hash, rb_str_new2(D_BUS), stoi(rb_str_new2(bus->dirname)) );
            rb_hash_aset(item_hash, rb_str_new2(D_DEV), stoi(rb_str_new2(dev->filename)) );
            rb_hash_aset(item_hash, rb_str_new2("vendor_id"), INT2FIX(dev->descriptor.idVendor) );
            rb_hash_aset(item_hash, rb_str_new2("product_id"), INT2FIX(dev->descriptor.idProduct) );
            rb_hash_aset(item_hash, rb_str_new2(D_MODEL), rb_str_new2(deviceinfo.Model) );
            
            rb_ary_push(devices, item_hash);
            num_devices += 1;
            
            CR(ptp_closesession(&params), "Could not close session!\n");
            close_usb(&ptp_usb, dev);
          }
        }
      }
    }
  }
  rb_iv_set(rb_cPTP, D_DEVICES, devices);
  return devices;
}

VALUE t_devices(VALUE self) {
  return rb_iv_get(rb_cPTP, D_DEVICES);
}

VALUE t_files_bang(VALUE self, VALUE device_name) {
  int busn = -1;
  int devn = -1;
  VALUE arr, devices;
  
  PTPParams params;
  PTP_USB ptp_usb;
  struct usb_device *dev;
  int i;
  PTPObjectInfo oi;
  struct tm *tm;
  
  devices = rb_iv_get(rb_cPTP, D_DEVICES);
  
  for (i = 0; i < num_devices; i++) {
    VALUE camera = rb_ary_entry(devices, i);
    
    // printf("%s \n", STR2CSTR(rb_hash_aref(camera, D_MODEL)) );
    // printf("%s \n", STR2CSTR(device_name) );
    
    if (rb_equal(rb_hash_aref(camera, rb_str_new2(D_MODEL)), device_name) == Qtrue) {
      busn = FIX2INT(rb_hash_aref(camera, rb_str_new2(D_BUS)));
      devn = FIX2INT(rb_hash_aref(camera, rb_str_new2(D_DEV)));
    }
  }
  
  // printf("%d/%d\n\n", busn, devn);
  
  if (busn == -1 || devn == -1)
    return Qnil;
  
  if (open_camera(busn, devn, 0, &ptp_usb, &params, &dev) < 0)
    return Qnil;
    
  arr = rb_ary_new();
  
  CR(ptp_getobjecthandles (&params,0xffffffff, 0x000000, 0x000000, &params.handles),"Could not get object handles\n");
  
  for (i = 0; i < params.handles.n; i++) {
    CR(ptp_getobjectinfo(&params,params.handles.Handler[i],&oi),"Could not get object info\n");
    
    if (oi.ObjectFormat == PTP_OFC_Association)
      continue;
      
    tm=gmtime(&oi.CaptureDate);
    
    // printf("0x%08lx: %12u\t%4i-%02i-%02i %02i:%02i\t%s\n",
    //   (long unsigned)params.handles.Handler[i],
    //   (unsigned) oi.ObjectCompressedSize, 
    //   tm->tm_year+1900, tm->tm_mon+1,tm->tm_mday,
    //   tm->tm_hour, tm->tm_min,
    //   oi.Filename);
    
    VALUE file_arr = rb_ary_new();
    rb_ary_push(file_arr, INT2FIX(params.handles.Handler[i]));
    rb_ary_push(file_arr, rb_str_new2(oi.Filename));
    rb_ary_push(file_arr, INT2FIX(oi.ObjectCompressedSize));
    rb_ary_push(file_arr, INT2FIX(tm->tm_year+1900));
    rb_ary_push(file_arr, INT2FIX(tm->tm_mon+1));
    rb_ary_push(file_arr, INT2FIX(tm->tm_mday));
    rb_ary_push(file_arr, INT2FIX(tm->tm_hour));
    rb_ary_push(file_arr, INT2FIX(tm->tm_min));
    rb_ary_push(file_arr, INT2FIX(tm->tm_sec));
    
    VALUE obj = rb_eval_string("PTP4R::Image.new()");
    rb_apply(obj, rb_intern("data="), file_arr);
    
    rb_ary_push(arr, obj);
  }
  
  close_camera(&ptp_usb, &params, dev);
  
  rb_iv_set(rb_cPTP, D_FILES, arr);
  return arr;
}

static VALUE t_init(VALUE self) {
  t_scan(self);
  return self;
}

static VALUE t_files(VALUE self, VALUE device) {
  if (rb_iv_get(rb_cPTP, D_FILES) == Qnil) {
    t_files_bang(self, device);
  }
  
  return rb_iv_get(rb_cPTP, D_FILES);
}

void Init_ptp4r_ext() {
  VALUE rb_mPTP = rb_define_module("PTP4R");  
  rb_cPTP = rb_define_class_under (rb_mPTP, "Ext", rb_cObject);
  
  rb_define_method(rb_cPTP, "initialize", t_init, 0);
  rb_define_method(rb_cPTP, "scan!", t_scan, 0);
  
  rb_define_method(rb_cPTP, "files!", t_files_bang, 1);
  rb_define_method(rb_cPTP, "files", t_files, 1);
  
  rb_define_method(rb_cPTP, "devices", t_devices, 0);
}