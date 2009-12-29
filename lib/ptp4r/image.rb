module PTP4R
  
  class Image
    require 'time'
    
    attr_accessor :handle, :size, :time, :filename
    
    def initialize(d = nil)
      data = d unless d.nil?
    end
    
    def data=(*d)
      @handle = d.shift
      @filename = d.shift
      @size = d.shift
      
      @time = Time.parse("#{d[0]}/#{d[1]}/#{d[2]} #{d[3]}:#{d[4]}:#{d[5]}")
    end
    
  end
end