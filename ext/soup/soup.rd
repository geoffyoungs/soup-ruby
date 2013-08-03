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

== flags Soup::Flags
== class Soup::Message
--- Soup::Message.new(String method, String uri)


--- Soup::Message#set_request_header(String name, String value)


--- Soup::Message#unset_request_header(String name)


--- Soup::Message#set_request_body(String type, String body)


--- Soup::Message#set_response(String content_type, String body)


--- Soup::Message#set_status(Integer status_code, String reason)


--- Soup::Message#set_redirect(Integer status_code, String redirect_uri)


--- Soup::Message#is_keepalive?

--- Soup::Message#flags=(SoupMessageFlags flags)


--- Soup::Message#flags

--- Soup::Message#get_response_header(String name)


--- Soup::Message#each_response_header

--- Soup::Message#response_body

