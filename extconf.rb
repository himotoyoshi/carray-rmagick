require 'mkmf'
require 'carray/mkmf'

begin
  have_image_magick = true
  $CPPFLAGS << " " << `Magick-config --cflags --cppflags`.split("\n").join(" ")
  `Magick-config --libs`.chomp.split(/\s+/).uniq.each do |opt|
    case opt
    when /\-L(.*)/
      $LDFLAGS << " " << opt
    end
  end
  `Magick-config --libs`.chomp.split(/\s+/).uniq.each do |opt|
    case opt
    when /\-l(.*)/
      unless have_library($1)
        have_image_magick = false
        break
      end
    end
  end
rescue
end

if have_carray()
  if have_header("magick/api.h") and have_image_magick
    create_makefile("carray/carray_rmagick")
  else
    open("Makefile", "w") { |io|
      io << "all:" << "\n"
      io << "install:" << "\n"        
      io << "clean:" << "\n"
      io << "distclean:" << "\n"     
      io << "\trm -rf mkmf.log Makefile" << "\n"     
    }
  end
end