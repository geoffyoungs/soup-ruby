#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
/* Includes */
#include <ruby.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "libsoup/soup.h"

/* Setup types */
/* Try not to clash with other definitions of bool... */
typedef int rubber_bool;
#define bool rubber_bool

/* Prototypes */
#include "rbglib.h"

#include "rbgtk.h"

#if defined(G_PLATFORM_WIN32) && !defined(RUBY_GTK2_STATIC_COMPILATION)
#  ifdef RUBY_GTK2_COMPILATION
#    define RUBY_GTK2_VAR __declspec(dllexport)
#  else
#    define RUBY_GTK2_VAR extern __declspec(dllimport)
#  endif
#else
#  define RUBY_GTK2_VAR extern
#endif

RUBY_GTK2_VAR VALUE mGtk;
RUBY_GTK2_VAR VALUE mGdk;

#define RBGTK_INITIALIZE(obj,gtkobj) (rbgtk_initialize_gtkobject(obj, GTK_OBJECT(gtkobj)))
static VALUE mSoup;
static VALUE cSession;
static VALUE
Session_queue(VALUE self, VALUE __v_message);
static VALUE
Session_send(VALUE self, VALUE __v_msg);
static VALUE
Session_pause(VALUE self, VALUE __v_msg);
static VALUE
Session_unpause(VALUE self, VALUE __v_msg);
static VALUE
Session_abort(VALUE self);
static VALUE cSessionAsync;
static VALUE
SessionAsync_initialize(VALUE self);
static VALUE cURI;
static VALUE
URI_initialize(VALUE self, VALUE __v_uri_string);
static VALUE
URI_to_s(VALUE self);
static VALUE cMessage;
static VALUE
Message_initialize(VALUE self, VALUE __v_method, VALUE __v_uri);
static VALUE
Message_set_request_header(VALUE self, VALUE __v_name, VALUE __v_value);
static VALUE
Message_unset_request_header(VALUE self, VALUE __v_name);
static VALUE
Message_set_request_body(VALUE self, VALUE __v_type, VALUE body);
static VALUE
Message_get_response_header(VALUE self, VALUE __v_name);
static VALUE
Message_each_response_header(VALUE self);
static VALUE
Message_response_body(VALUE self);
static VALUE _gcpool_RubyFunc = Qnil;
static void __gcpool_RubyFunc_add(VALUE val);
static void __gcpool_RubyFunc_del(VALUE val);
#define RUBYFUNC_ADD(val) __gcpool_RubyFunc_add(val)
#define RUBYFUNC_DEL(val) __gcpool_RubyFunc_del(val)

/* Inline C code */

#include <intern.h>

#define INIT(obj, gobj) __my_init(obj, G_OBJECT(gobj))

static inline void __my_init(VALUE obj, GObject *gobj) {
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


/* Code */
static VALUE
Session_queue(VALUE self, VALUE __v_message)
{
  SoupMessage * message; SoupMessage * __orig_message;
  SoupSession *_self = ((SoupSession*)RVAL2GOBJ(self));
  __orig_message = message = RVAL2GOBJ(__v_message);

#line 52 "/home/geoff/Projects/soup/ext/soup/soup.cr"

  do {
  if (rb_block_given_p()) { VALUE  block  =
 rb_block_proc();
  RUBYFUNC_ADD(block);
  soup_session_queue_message(_self, message, queue_callback, (void*)block);
  } else { soup_session_queue_message(_self, message, NULL, NULL);
  }

  } while(0);

  return Qnil;
}

static VALUE
Session_send(VALUE self, VALUE __v_msg)
{
  VALUE __p_retval = Qnil;
  SoupMessage * msg; SoupMessage * __orig_msg;
  SoupSession *_self = ((SoupSession*)RVAL2GOBJ(self));
  __orig_msg = msg = RVAL2GOBJ(__v_msg);

#line 62 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  do { __p_retval =  UINT2NUM(soup_session_send_message(_self, msg)); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Session_pause(VALUE self, VALUE __v_msg)
{
  SoupMessage * msg; SoupMessage * __orig_msg;
  SoupSession *_self = ((SoupSession*)RVAL2GOBJ(self));
  __orig_msg = msg = RVAL2GOBJ(__v_msg);

#line 66 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_session_pause_message(_self, msg);
 
  return Qnil;
}

static VALUE
Session_unpause(VALUE self, VALUE __v_msg)
{
  SoupMessage * msg; SoupMessage * __orig_msg;
  SoupSession *_self = ((SoupSession*)RVAL2GOBJ(self));
  __orig_msg = msg = RVAL2GOBJ(__v_msg);

#line 70 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_session_unpause_message(_self, msg);
 
  return Qnil;
}

static VALUE
Session_abort(VALUE self)
{
  SoupSession *_self = ((SoupSession*)RVAL2GOBJ(self));

#line 74 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_session_abort(_self);
 
  return Qnil;
}

static VALUE
SessionAsync_initialize(VALUE self)
{

#line 81 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  INIT(self, soup_session_async_new());
 
  return Qnil;
}

static VALUE
URI_initialize(VALUE self, VALUE __v_uri_string)
{
  char * uri_string; char * __orig_uri_string;
  __orig_uri_string = uri_string = ( NIL_P(__v_uri_string) ? NULL : StringValuePtr(__v_uri_string) );

#line 88 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  INIT(self, soup_uri_new(uri_string));
 
  return Qnil;
}

static VALUE
URI_to_s(VALUE self)
{
  VALUE __p_retval = Qnil;
  SoupURI * _self = ((SoupURI *)RVAL2BOXED(self, SOUP_TYPE_URI));

#line 91 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  do { __p_retval =  rb_str_new2(soup_uri_to_string(_self, FALSE)); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Message_initialize(VALUE self, VALUE __v_method, VALUE __v_uri)
{
  char * method; char * __orig_method;
  char * uri; char * __orig_uri;
  __orig_method = method = ( NIL_P(__v_method) ? NULL : StringValuePtr(__v_method) );
  __orig_uri = uri = ( NIL_P(__v_uri) ? NULL : StringValuePtr(__v_uri) );

#line 98 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  INIT(self, soup_message_new(method, uri));
 
  return Qnil;
}

static VALUE
Message_set_request_header(VALUE self, VALUE __v_name, VALUE __v_value)
{
  char * name; char * __orig_name;
  char * value; char * __orig_value;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_name = name = ( NIL_P(__v_name) ? NULL : StringValuePtr(__v_name) );
  __orig_value = value = ( NIL_P(__v_value) ? NULL : StringValuePtr(__v_value) );

#line 102 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_headers_replace(_self->request_headers, name, value);
 
  return self;
}

static VALUE
Message_unset_request_header(VALUE self, VALUE __v_name)
{
  char * name; char * __orig_name;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_name = name = ( NIL_P(__v_name) ? NULL : StringValuePtr(__v_name) );

#line 106 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_headers_remove(_self->request_headers, name);
 
  return Qnil;
}

static VALUE
Message_set_request_body(VALUE self, VALUE __v_type, VALUE body)
{
  char * type; char * __orig_type;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_type = type = ( NIL_P(__v_type) ? NULL : StringValuePtr(__v_type) );
  Check_Type(body, T_STRING);

#line 110 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_set_request(_self, type, SOUP_MEMORY_COPY, RSTRING_PTR(body), RSTRING_LEN(body));
 
  return self;
}

static VALUE
Message_get_response_header(VALUE self, VALUE __v_name)
{
  VALUE __p_retval = Qnil;
  char * name; char * __orig_name;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_name = name = ( NIL_P(__v_name) ? NULL : StringValuePtr(__v_name) );

#line 114 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  do { __p_retval =  rb_str_new2(soup_message_headers_get_one(_self->response_headers, name)); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Message_each_response_header(VALUE self)
{
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));

#line 118 "/home/geoff/Projects/soup/ext/soup/soup.cr"

  do {
  VALUE  block  =
 rb_block_proc();
  rb_need_block();
  soup_message_headers_foreach(_self->response_headers, each_header, (gpointer)block);
 

  } while(0);

  return Qnil;
}

static VALUE
Message_response_body(VALUE self)
{
  VALUE __p_retval = Qnil;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));

#line 124 "/home/geoff/Projects/soup/ext/soup/soup.cr"

  do {
  SoupBuffer * buffer  =
 soup_message_body_flatten(_self->response_body);
  volatile VALUE  str  =
 rb_str_new(buffer->data, buffer->length);
  soup_buffer_free(buffer);
  do { __p_retval = str; goto out; } while(0);

  } while(0);

out:
  return __p_retval;
}

static void __gcpool_RubyFunc_add(VALUE val)
    {
      if (_gcpool_RubyFunc == Qnil)
      {
        _gcpool_RubyFunc = rb_ary_new3(1, val);
      }
      else
      {
        rb_ary_push(_gcpool_RubyFunc, val);
      }
    }
    
    static void __gcpool_RubyFunc_del(VALUE val)
    {
      if (_gcpool_RubyFunc == Qnil)
      {
        rb_warn("Trying to remove object from empty GC queue RubyFunc");
        return;
      }
      rb_ary_delete(_gcpool_RubyFunc, val);
      // If nothing is referenced, don't keep an empty array in the pool...
      if (RARRAY_LEN(_gcpool_RubyFunc) == 0)
        _gcpool_RubyFunc = Qnil;
    }
    
/* Init */
void
Init_soup(void)
{
  mSoup = rb_define_module("Soup");
  cSession = G_DEF_CLASS(SOUP_TYPE_SESSION, "Session", mSoup);
  rb_define_method(cSession, "queue", Session_queue, 1);
  rb_define_method(cSession, "send", Session_send, 1);
  rb_define_method(cSession, "pause", Session_pause, 1);
  rb_define_method(cSession, "unpause", Session_unpause, 1);
  rb_define_method(cSession, "abort", Session_abort, 0);
  cSessionAsync = G_DEF_CLASS(SOUP_TYPE_SESSION_ASYNC, "SessionAsync", mSoup);
  rb_define_method(cSessionAsync, "initialize", SessionAsync_initialize, 0);
  cURI = G_DEF_CLASS(SOUP_TYPE_URI, "URI", mSoup);
  rb_define_method(cURI, "initialize", URI_initialize, 1);
  rb_define_method(cURI, "to_s", URI_to_s, 0);
  cMessage = G_DEF_CLASS(SOUP_TYPE_MESSAGE, "Message", mSoup);
  rb_define_method(cMessage, "initialize", Message_initialize, 2);
  rb_define_method(cMessage, "set_request_header", Message_set_request_header, 2);
  rb_define_method(cMessage, "unset_request_header", Message_unset_request_header, 1);
  rb_define_method(cMessage, "set_request_body", Message_set_request_body, 2);
  rb_define_method(cMessage, "get_response_header", Message_get_response_header, 1);
  rb_define_method(cMessage, "each_response_header", Message_each_response_header, 0);
  rb_define_method(cMessage, "response_body", Message_response_body, 0);
rb_gc_register_address(&_gcpool_RubyFunc);
}
