= module Soup
== class Soup::Session
--- Soup::Session#queue(SoupMessage* message)


--- Soup::Session#send(SoupMessage* msg)


--- Soup::Session#pause(SoupMessage* msg)


--- Soup::Session#unpause(SoupMessage* msg)


--- Soup::Session#abort

== class Soup::SessionAsync
--- Soup::SessionAsync.new

== class Soup::URI
--- Soup::URI.new(String uri_string)


--- Soup::URI#to_s

== class Soup::Message
--- Soup::Message.new(String method, String uri)


--- Soup::Message#set_request_header(String name, String value)


--- Soup::Message#unset_request_header(String name)


--- Soup::Message#set_request_body(String type, String body)


--- Soup::Message#get_response_header(String name)


--- Soup::Message#each_response_header

--- Soup::Message#response_body

