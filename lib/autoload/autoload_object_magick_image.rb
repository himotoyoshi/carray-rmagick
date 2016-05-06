autoload :CAMagickImage, "carray/object/ca_obj_magick_image"

module Magick
  class Image
    magick_image_rb = "carray/object/ca_obj_magick_image"
    autoload_method "ca", magick_image_rb
    autoload_method "to_ca", magick_image_rb
  end
end
