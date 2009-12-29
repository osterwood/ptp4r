require 'mkmf'

unless find_header('usb.h', "/opt/local/include/") and find_library('usb', nil, "/opt/local/lib/")
  puts "ERROR : USB library not found"
  exit 0
end

create_makefile("ptp4r_ext")