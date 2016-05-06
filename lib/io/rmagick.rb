# ----------------------------------------------------------------------------
#
#  carray/io/rmagick.rb
#
#  This file is part of Ruby/CArray extension library.
#  You can redistribute it and/or modify it under the terms of
#  the Ruby Licence.
#
#  Copyright (C) 2005-2008  Hiroki Motoyoshi
#
# ----------------------------------------------------------------------------

require "RMagick"
require "carray/carray_rmagick"

class Magick::Image

  def export_pixels_to_ca (ca, map="RGB",x=0,y=0)
    case ca.data_type
    when CA_FIXLEN
      case ca.bytes 
      when map.size
        stype = Magick::CharPixel
      when 2*map.size
        stype = Magick::ShortPixel        
      when 4*map.size
        stype = Magick::IntegerPixel        
      else
        raise
      end
    when CA_UINT8, CA_INT8
      stype = Magick::CharPixel
    when CA_UINT16, CA_INT16
      stype = Magick::ShortPixel
    when CA_UINT32, CA_INT32
      stype = Magick::IntegerPixel
    when CA_FLOAT32
      stype = Magick::FloatPixel
    when CA_FLOAT64
      stype = Magick::DoublePixel
    else
      raise "invalid strage type"
    end
    case ca.rank 
    when 2
      if map.size != 1
        raise
      end
    when 3
      if map.size != ca.dim.last
        raise
      end
    else  
      raise "invalid rank"
    end
    ca.load_binary(export_pixels_to_str(x,y,ca.dim1,ca.dim0,map,stype))
    return ca
  end

  def import_pixels_from_ca (ca,map="RGBO",x=0,y=0)
    case ca.data_type
    when CA_FIXLEN
      case ca.bytes 
      when map.size
        stype = Magick::CharPixel
      when 2*map.size
        stype = Magick::ShortPixel        
      when 4*map.size
        stype = Magick::IntegerPixel        
      else
        raise
      end
    when CA_UINT8, CA_INT8
      stype = Magick::CharPixel
    when CA_UINT16, CA_INT16
      stype = Magick::ShortPixel
    when CA_UINT32, CA_INT32
      stype = Magick::IntegerPixel
    when CA_FLOAT32
      stype = Magick::FloatPixel
    when CA_FLOAT64
      stype = Magick::DoublePixel
    else
      raise "invalid strage type"
    end
    import_pixels(x,y,ca.dim1,ca.dim0,map,ca.dump_binary,stype)
    return self
  end
  
end

class CArray

  def self.load_image (file, map="RGBO", data_type = nil)
    img = Magick::Image.read(file).first
    rows, cols = img.rows, img.columns
    unless data_type
      case img.quantum_depth
      when 8
        data_type = CA_UINT8
      when 16
        data_type = CA_UINT16
      when 32
        data_type = CA_UINT32
      end
    end
    case map.size
    when 1
      out = CArray.new(data_type, [rows, cols])
    else
      out = CArray.new(data_type, [rows, cols, map.size])
    end
    img.export_pixels_to_ca(out, map)
    return out
  end

  def to_image (map="RGBO", ch=nil)
    img = Magick::Image.new(dim1, dim0)
    case data_type
    when CA_FIXLEN
      img.import_pixels_from_ca(self, map)
    else
      case rank
      when 2
        map = "I"
        ca = self[:%, :%, 1]
      when 3
        unless ch
          ch = Array.new(map.size){|i| i}
        end
        ca = self[nil,nil,CA_INT(ch)]
      end
      img.import_pixels_from_ca(ca, map)
    end
    return img
  end

  def save_image (file, map="RGBO", ch=nil)
    to_image(map, ch).write(file)
    return nil
  end

  def display_image (map="RGBO", ch=nil)
    to_image(map, ch).display
  end
end

