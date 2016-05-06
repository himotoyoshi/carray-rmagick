module Magick
  class Image
    rmagick_rb = "carray/io/rmagick"
    autoload_method "export_pixels_to_ca", rmagick_rb
    autoload_method "import_pixels_from_ca", rmagick_rb
  end
end

class CArray
  rmagick_rb = "carray/io/rmagick"  
  autoload_method "self.load_image", rmagick_rb
  autoload_method "save_image", rmagick_rb
  autoload_method "display_image", rmagick_rb
  autoload_method "to_image", rmagick_rb
end
  
  
