require 'pp'

module PTP4R
  
  class Camera
    
    attr_accessor :model, :bus, :dev, :product_id, :vendor_id
    
    def initialize(params)
      # {"model"=>"Apple iPhone", "bus"=>253, "product_id"=>4756, "vendor_id"=>1452, "dev"=>2}
      pp params
      
      @model = params['model']
      @bus = params['bus']
      @dev = params['dev']
      @product_id = params['product_id']
      @vendor_id = params['vendor_id']
    end
    
    def files
      PTP4R.link.files(@bus, @dev)
    end
  end
  
end