# ----------------------------------------------------------------------------
#
#  carray/object/ca_obj_magick_image.rb
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

class CAMagickImage < CAObject

  DEPTH_TO_TYPE = { 
    8 => CA_UINT8,
    16 => CA_UINT16,
    32 => CA_UINT32
  }

  def initialize (image)
    @image = image
    if Magick.const_defined?(:QuantumDepth)
      magick_quantum_depth = Magick::QuantumDepth
    else
      magick_quantum_depth = Magick::MAGICKCORE_QUANTUM_DEPTH      
    end
    @qtype = DEPTH_TO_TYPE[magick_quantum_depth]
    type   = DEPTH_TO_TYPE[@image.quantum_depth]
    @scale = 0
    (magick_quantum_depth / @image.quantum_depth).times do |i|
      @scale += 1 << @image.quantum_depth * i
    end
    super(type, [@image.rows, @image.columns, 4])
  end
  
  def fetch_index (idx)
    return @image._fetch_index(idx)/@scale
  end

  def store_index (idx, val)
    return @image._store_index(idx, val*@scale)
  end
  
  def copy_data (data)
    qdata = CArray.wrap_writable(data, @qtype)
    qdata.attach! {
      @image._copy_data_to_ca(qdata)
      if @scale != 1
        qdata.div!(@scale)
      end
    }
  end

  def sync_data (data)
    qdata = CArray.wrap_readonly(data, @qtype)
    qdata.attach {
      if @scale != 1
        qdata.mul!(@scale)
      end 
      @image._sync_data_from_ca(qdata)
    }
  end
  
  def fill_data (val)
    return @image._fill_data(val*@scale)
  end
  
end

class Magick::Image

  def ca
    return CAMagickImage.new(self)
  end

  def to_ca
    return ca.to_ca
  end

end