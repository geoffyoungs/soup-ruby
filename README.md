LibSoup bindings
================

Requires
--------
* rubber-generate >= 0.0.15
* ruby-gnome2 (including development files)
* libsoup-2.4 (including development files)

Description
-----------

Basic bindings for using the libsoup library from Ruby.

e.g

	require 'gtk2'
	require 'soup'
	require 'digest/md5'

	session = Soup::SessionAsync.new

	message = Soup::Message.new("GET", "http://www.google.com/")

	session.queue(message) do |*args|
		puts message.response_body

		Gtk.main_quit

		false
	end

	Gtk.main

