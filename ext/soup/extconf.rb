require 'mkmf'
use_gems = false
begin
  require 'mkmf-gnome2'
rescue LoadError
  use_gems = true
end

if use_gems or Object.const_defined?('Gem')
  require 'rubygems'
  gem 'glib2'
  require 'mkmf-gnome2'
  %w[rbglib.h rbgtk.h rbpango.h rbatk.h].each do |header|
  	Gem.find_files(header).each do |f|
		$CFLAGS += " '-I#{File.dirname(f)}'"
	end
  end
end
# Look for headers in {gem_root}/ext/{package}
if use_gems
  %w[
 glib2 gdk_pixbuf2 atk gtk2].each do |package|
    require package
    if Gem.loaded_specs[package]
      $CFLAGS += " -I" + Gem.loaded_specs[package].full_gem_path + "/ext/" + package
    else
      if fn = $".find { |n| n.sub(/[.](so|rb)$/,'') == package }
        dr = $:.find { |d| File.exist?(File.join(d, fn)) }
        pt = File.join(dr,fn) if dr && fn
      else
        pt = "??"
      end
      STDERR.puts "require '" + package + "' loaded '"+pt+"' instead of the gem - trying to continue, but build may fail"
    end
  end
end
if RbConfig::CONFIG.has_key?('rubyhdrdir')
  $CFLAGS += " -I" + RbConfig::CONFIG['rubyhdrdir']+'/ruby'
end

$CFLAGS += " -I."
have_func("rb_errinfo")
PKGConfig.have_package("gtk+-2.0") or exit(-1)
PKGConfig.have_package("libsoup-2.4") or exit(-1)

unless have_header("libsoup/soup.h")
  paths = Gem.find_files("libsoup/soup.h")
  paths.each do |path|
    $CFLAGS += " '-I#{File.dirname(path)}'"
  end
  have_header("libsoup/soup.h") or exit -1
end

STDOUT.print("checking for new allocation framework... ") # for ruby-1.7
if Object.respond_to? :allocate
  STDOUT.print "yes
"
  $defs << "-DHAVE_OBJECT_ALLOCATE"
else
  STDOUT.print "no
"
end

top = File.expand_path(File.dirname(__FILE__) + '/..') # XXX
$CFLAGS += " " + ['glib/src'].map{|d|
  "-I" + File.join(top, d)
}.join(" ")

have_func("rb_define_alloc_func") # for ruby-1.8

#set_output_lib('libruby-soup.a')
if /cygwin|mingw/ =~ RUBY_PLATFORM
  top = "../.."
  [
    ["glib/src", "ruby-glib2"],
  ].each{|d,l|
    $LDFLAGS << sprintf(" -L%s/%s", top, d)
    $libs << sprintf(" -l%s", l)
  }
end
begin
  srcdir = File.expand_path(File.dirname($0))

  begin

    obj_ext = "."+$OBJEXT

    $libs = $libs.split(/ /).uniq.join(' ')
    $source_files = Dir.glob(sprintf("%s/*.c", srcdir)).map{|fname|
      fname[0, srcdir.length+1] = ''
      fname
    }
    $objs = $source_files.collect do |item|
      item.gsub(/.c$/, obj_ext)
    end

    #
    # create Makefile
    #
    $defs << "-DRUBY_SOUP_COMPILATION"
    # $CFLAGS << $defs.join(' ')
    create_makefile("soup", srcdir)
    raise Interrupt if not FileTest.exist? "Makefile"

    File.open("Makefile", "a") do |mfile|
      $source_files.each do |e|
        mfile.print sprintf("%s: %s
", e.gsub(/.c$/, obj_ext), e)
      end
    end
  ensure
    #Dir.chdir ".."
  end

  #create_top_makefile()
rescue Interrupt
  print "  [error] " + $!.to_s + "
"
end

