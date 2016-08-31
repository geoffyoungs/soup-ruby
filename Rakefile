require 'rubygems'
gem 'rake-compiler'
require 'rake/extensiontask'
BASE_DIR = Dir.pwd
require 'rubygems/package_task'
require 'rake/testtask'

$: << './lib'

require 'soup/version'

namespace :prepare do
FileList["ext/*/*.cr"].each do |cr|
	dir = File.dirname(cr)
	name = File.basename(dir)
	desc "Generate source for #{name}"
	task(name.intern) do
		sh 'rubber-generate', '--build-dir', dir, cr
	end
end
end

spec = Gem::Specification.new do |s|
	s.name = "soup-ruby"
	s.author = "Geoff Youngs"
	s.email = "git@intersect-uk.co.uk"
	s.version = Soup::BINDINGS_VERSION
	s.homepage = "http://github.com/geoffyoungs/soup-ruby"
	s.summary = "Soup bindings using rubber-generate"
	s.add_dependency("rubber-generate", ">= 0.0.17")
	s.add_dependency("gtk2", ">= 2.0.0")
	s.platform = Gem::Platform::RUBY
	s.extensions = FileList["ext/*/extconf.rb"]
  s.licenses = ['The Ruby License']
	s.files = FileList['ext/*/*.{c,h,cr,rd,rb}'] + ['Rakefile', 'README.md'] + FileList['lib/**/*.rb']
s.description = <<-EOF
Libsoup bindings for ruby.  Partial coverage sufficient to allow HTTP requests to be handled without blocking the mainloop.

e.g
require 'gtk2'
require 'soup'


message = Soup::Message.new("GET", "http://www.example.com/")
Soup::SessionAsync.new.queue(message) do |_sess,_mess|
  puts "Got response"
  Gtk.main_quit
end

Gtk.main
EOF
end
Gem::PackageTask.new(spec) do |pkg|
    pkg.need_tar = true
end
Rake::ExtensionTask.new("soup", spec)

Rake::TestTask.new do |t|
	t.test_files = FileList['test/*_test.rb']
end

task :default, :compile

