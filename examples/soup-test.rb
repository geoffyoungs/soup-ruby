require 'rubygems'	
require 'glib2'
$: << 'x86_64-linux'
require 'soup'
require 'gtk2'

session = Soup::SessionAsync.new

message = Soup::Message.new("GET", "http://www.google.com/")

session.queue(message) do |*args|
	p [:done, args]
	message.each_response_header do |header, value|
		p [header, value]
	end
	p message.response_body
	Gtk.main_quit
	false
end

Gtk.main

