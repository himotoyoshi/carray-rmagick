
Gem::Specification::new do |s|
  version = "1.0.0"

  files = Dir.glob("**/*") - [ 
                               Dir.glob("carray*.gem"), 
                             ].flatten

  s.platform    = Gem::Platform::RUBY
  s.name        = "carray-rmagick"
  s.summary     = "Extension for converting Magick::Image to/from CArray"
  s.description = <<-HERE
    Extension for converting Magick::Image to/from CArray
  HERE
  s.version     = version
  s.author      = "Hiroki Motoyoshi"
  s.email       = ""
  s.homepage    = 'https://github.com/himotoyoshi/carray_rmagick'
  s.files       = files
  s.extensions  = [ "extconf.rb" ]
  s.has_rdoc    = false
  s.required_ruby_version = ">= 1.8.1"
end
