%pkg-config gtk+-2.0
%pkg-config libsoup-2.4
%name soup

%{
#include <intern.h>

#define INIT(obj, gobj) __my_init(obj, G_OBJECT(gobj))

static inline __my_init(VALUE obj, GObject *gobj) {
	g_object_ref(gobj);
	g_object_ref_sink(gobj);
	G_INITIALIZE(obj, gobj);
}

static inline VALUE strOrNil(const char *str) {
	if (str) {
		return rb_str_new2(str);
	} else {
		return Qnil;
	}
}

static void queue_callback(SoupSession *session, SoupMessage *message, gpointer user_data) {
	VALUE block = (VALUE)user_data;
	volatile VALUE ary = rb_ary_new3(2, GOBJ2RVAL(session), GOBJ2RVAL(message));
	if (RTEST(rb_proc_call(block, ary))) {
		// Truthy - requeue
		soup_session_requeue_message(session, message);
	} else {
		RUBYFUNC_DEL(block);
	}
}

static void each_header(const char *name, const char *value, gpointer user_data) {
	VALUE block = (VALUE)user_data;
	volatile VALUE ary = rb_ary_new3(2, rb_str_new2(name), rb_str_new2(value));
	rb_proc_call(block, ary);
}

%}

%include libsoup/soup.h

%map strOrNil > VALUE : strOrNil(%%)

module Soup
	gcpool RubyFunc

	gobject Session < SOUP_TYPE_SESSION
		@type SoupSession
		def queue(SoupMessage *message)
			if (rb_block_given_p()) {
				VALUE block = rb_block_proc();
				RUBYFUNC_ADD(block);
				soup_session_queue_message(_self, message, queue_callback, (void*)block);
			} else {
				soup_session_queue_message(_self, message, NULL, NULL);				
			}
		end

		def abort
			soup_session_abort(_self);
		end
	end

	gobject SessionAsync < SOUP_TYPE_SESSION_ASYNC
		@type SoupAsyncSession
		def initialize()
			INIT(self, soup_session_async_new());
		end
	end

	gboxed URI < SOUP_TYPE_URI
		@type SoupURI
		def initialize(char * uri_string)
			INIT(self, soup_uri_new(uri_string));
		end
		def char*:to_s
			return soup_uri_to_string(_self, FALSE);
		end
	end

	gobject Message < SOUP_TYPE_MESSAGE
		@type SoupMessage
		def initialize(char * method, char * uri)
			INIT(self, soup_message_new(method, uri));
		end

		def char *:get_response_header(char *name)
			return soup_message_headers_get_one(_self->response_headers, name);
		end

		def each_response_header
			VALUE block = rb_block_proc();
			rb_need_block();
			soup_message_headers_foreach(_self->response_headers, each_header, (gpointer)block);
		end

		def response_body
			SoupBuffer *buffer = soup_message_body_flatten(_self->response_body);
			volatile VALUE str = rb_str_new(buffer->data, buffer->length);

			soup_buffer_free(buffer);
			return str;
		end
	end
end

