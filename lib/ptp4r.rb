$:.unshift File.expand_path(File.dirname(__FILE__))

require 'ptp4r_ext'
require 'ptp4r/image'
require 'ptp4r/camera'

require 'lib'

module PTP4R
  
  mattr_accessor :link
  
  class Base
    
    attr_reader :devices
    
    def initialize
      PTP4R.link = Ext.new
      
      @devices = link.devices.map{|device| Camera.new(device) } unless link.devices.nil?
    end
    
    def link
      PTP4R.link
    end
    
    def device_by_model(model)
      devices.nil? ? nil : devices.select{|d| d.model == model}.first
    end
    
  end
end