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
static VALUE flagsMessageFlags;

static VALUE flagsBaseClass;

typedef struct {
	int value;
	char *name;
	char *fullname;
} FlagsData;

static VALUE make_flags_value(VALUE klass, int value, char *name, char *fullname)
{
	FlagsData *data = NULL;
	
	data = ALLOC(FlagsData);
	data->value = value;
	data->name = name;
	data->fullname = fullname;
	
	return Data_Wrap_Struct(klass, NULL, free, data);
}
static int flags_value_to_int(VALUE value, VALUE klass)
{
	switch (TYPE(value))
	{
	case T_FIXNUM:
	case T_FLOAT:
		return NUM2INT(value);
	break;
	case T_DATA:
		if (rb_obj_is_kind_of(value, flagsBaseClass))
		{
			FlagsData *data = NULL;
			
			if ((klass != Qnil) && (!rb_obj_is_kind_of(value, klass)))
			{
				rb_raise(rb_eTypeError, "Wrong type of flags  %s (%s required)", rb_obj_classname(value), rb_class2name(klass));
			}
			
			Data_Get_Struct(value, FlagsData, data);
			return data->value;
		}
	break;
	}
	return 0;
	
}

static VALUE rubber_flags_inspect(VALUE value)
{
	FlagsData *data = NULL;
	volatile VALUE str = rb_str_new("#<", 2);
	char number[16] = ""; 
	
	Data_Get_Struct(value, FlagsData, data);
	
	rb_str_cat2(str, rb_obj_classname(value));
	rb_str_cat2(str, " - ");
	rb_str_cat2(str, data->name);
	rb_str_cat2(str, "(");
	sprintf(number, "%i", data->value);
	rb_str_cat2(str, number);
	rb_str_cat2(str, ")>");
	
	return str;
}

static VALUE rubber_flags_to_s(VALUE value)
{
	FlagsData *data = NULL;
	
	Data_Get_Struct(value, FlagsData, data);
	
	return rb_str_new2(data->fullname);
}
static VALUE rubber_flags_name(VALUE value)
{
	FlagsData *data = NULL;
	
	Data_Get_Struct(value, FlagsData, data);
	
	return rb_str_new2(data->name);
}

static VALUE rubber_flags_cmp(VALUE value, VALUE other)
{
	VALUE a,b;
	a = rb_funcall(value, rb_intern("to_i"), 0);
	b = rb_funcall(other, rb_intern("to_i"), 0);
#ifdef RB_NUM_COERCE_FUNCS_NEED_OPID
	return rb_num_coerce_cmp(a, b, rb_intern("=="));
#else
	return rb_num_coerce_cmp(a, b);
#endif
}

static VALUE rubber_flags_to_i(VALUE value)
{
	FlagsData *data = NULL;
	
	Data_Get_Struct(value, FlagsData, data);
	
	return INT2FIX(data->value);
}

static VALUE rubber_flags_coerce(VALUE value, VALUE other)
{
	FlagsData *data = NULL;
	
	Data_Get_Struct(value, FlagsData, data);
	
	switch(TYPE(other))
	{
	case T_FIXNUM:
	case T_BIGNUM:
		return INT2FIX(data->value);
	case T_FLOAT:
		return Qnil;
	default:
		return Qnil;
	}
}

static VALUE rubber_flags_and(VALUE value, VALUE other)
{
	FlagsData *data = NULL;
	int original = 0;
	int other_num = 0;
	
	Data_Get_Struct(value, FlagsData, data);

	original = data->value;

	other_num = flags_value_to_int(value, CLASS_OF(value));
	
//	return INT2NUM(original & other_num);
	return make_flags_value(CLASS_OF(value), original & other_num, "", "");
}

static VALUE rubber_flags_or(VALUE value, VALUE other)
{
	FlagsData *data = NULL;
	int original = 0;
	int other_num = 0;
	
	Data_Get_Struct(value, FlagsData, data);

	original = data->value;

	other_num = flags_value_to_int(value, CLASS_OF(value));
	
	return make_flags_value(CLASS_OF(value), original | other_num, "", "");
}



static VALUE flagsMessageFlags_SOUP_MESSAGE_NO_REDIRECT = Qnil;
static VALUE flagsMessageFlags_SOUP_MESSAGE_CAN_REBUILD = Qnil;
static VALUE flagsMessageFlags_SOUP_MESSAGE_CONTENT_DECODED = Qnil;
static VALUE flagsMessageFlags_SOUP_MESSAGE_CERTIFICATE_TRUSTED = Qnil;
static VALUE flagsMessageFlags_SOUP_MESSAGE_NEW_CONNECTION = Qnil;
	static VALUE rubber_flagsMessageFlags_flags_inspect(VALUE value)
{
	FlagsData *data = NULL;
	volatile VALUE str = rb_str_new("#<", 2);
	char number[16] = ""; 
	int c=0;
	
	Data_Get_Struct(value, FlagsData, data);
	
	rb_str_cat2(str, rb_obj_classname(value));
	rb_str_cat2(str, " - ");
		if ((data->value & SOUP_MESSAGE_NO_REDIRECT)==SOUP_MESSAGE_NO_REDIRECT) {
			if (c>0)
				rb_str_cat2(str, ", ");
			rb_str_cat2(str, "no-redirect");
			c ++;
		}
		if ((data->value & SOUP_MESSAGE_CAN_REBUILD)==SOUP_MESSAGE_CAN_REBUILD) {
			if (c>0)
				rb_str_cat2(str, ", ");
			rb_str_cat2(str, "can-rebuild");
			c ++;
		}
		if ((data->value & SOUP_MESSAGE_CONTENT_DECODED)==SOUP_MESSAGE_CONTENT_DECODED) {
			if (c>0)
				rb_str_cat2(str, ", ");
			rb_str_cat2(str, "content-decoded");
			c ++;
		}
		if ((data->value & SOUP_MESSAGE_CERTIFICATE_TRUSTED)==SOUP_MESSAGE_CERTIFICATE_TRUSTED) {
			if (c>0)
				rb_str_cat2(str, ", ");
			rb_str_cat2(str, "certificate-trusted");
			c ++;
		}
		if ((data->value & SOUP_MESSAGE_NEW_CONNECTION)==SOUP_MESSAGE_NEW_CONNECTION) {
			if (c>0)
				rb_str_cat2(str, ", ");
			rb_str_cat2(str, "new-connection");
			c ++;
		}
	rb_str_cat2(str, " (");
	sprintf(number, "%i", data->value);
	rb_str_cat2(str, number);
	rb_str_cat2(str, ")>");
	
	return str;
}
typedef int MessageFlags;
#ifdef __GNUC__
// No point in declaring these unless we're using GCC
// They're ahead of any code that uses them anyway.
static VALUE flags_MessageFlags_to_ruby(int value)
__attribute__ ((unused))
;
static int flags_ruby_to_MessageFlags(VALUE val)
__attribute__ ((unused))
;
#endif

static VALUE flags_MessageFlags_to_ruby(int value) { switch(value) {
    case SOUP_MESSAGE_NO_REDIRECT: return flagsMessageFlags_SOUP_MESSAGE_NO_REDIRECT;
    case SOUP_MESSAGE_CAN_REBUILD: return flagsMessageFlags_SOUP_MESSAGE_CAN_REBUILD;
    case SOUP_MESSAGE_CONTENT_DECODED: return flagsMessageFlags_SOUP_MESSAGE_CONTENT_DECODED;
    case SOUP_MESSAGE_CERTIFICATE_TRUSTED: return flagsMessageFlags_SOUP_MESSAGE_CERTIFICATE_TRUSTED;
    case SOUP_MESSAGE_NEW_CONNECTION: return flagsMessageFlags_SOUP_MESSAGE_NEW_CONNECTION;
	}; return make_flags_value(flagsMessageFlags, value, "various", "Various"); }
	static int flags_ruby_to_MessageFlags(VALUE val) { return flags_value_to_int(val, flagsMessageFlags); }
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
Message_set_response(VALUE self, VALUE __v_content_type, VALUE body);
static VALUE
Message_set_status(int __p_argc, VALUE *__p_argv, VALUE self);
static VALUE
Message_set_redirect(VALUE self, VALUE __v_status_code, VALUE __v_redirect_uri);
static VALUE
Message_is_keepalive_query(VALUE self);
static VALUE
Message_flags_equals(VALUE self, VALUE __v_flags);
static VALUE
Message_flags(VALUE self);
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

#line 100 "/home/geoff/Projects/soup/ext/soup/soup.cr"
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

#line 104 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_headers_replace(_self->request_headers, name, value);
 
  return self;
}

static VALUE
Message_unset_request_header(VALUE self, VALUE __v_name)
{
  char * name; char * __orig_name;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_name = name = ( NIL_P(__v_name) ? NULL : StringValuePtr(__v_name) );

#line 108 "/home/geoff/Projects/soup/ext/soup/soup.cr"
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

#line 112 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_set_request(_self, type, SOUP_MEMORY_COPY, RSTRING_PTR(body), RSTRING_LEN(body));
 
  return self;
}

static VALUE
Message_set_response(VALUE self, VALUE __v_content_type, VALUE body)
{
  char * content_type; char * __orig_content_type;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_content_type = content_type = ( NIL_P(__v_content_type) ? NULL : StringValuePtr(__v_content_type) );
  Check_Type(body, T_STRING);

#line 116 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_set_response(_self, content_type, SOUP_MEMORY_COPY, RSTRING_PTR(body), RSTRING_LEN(body));
 
  return self;
}

static VALUE
Message_set_status(int __p_argc, VALUE *__p_argv, VALUE self)
{
  VALUE __v_status_code = Qnil;
  int status_code; int __orig_status_code;
  VALUE __v_reason = Qnil;
  char * reason; char * __orig_reason;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));

  /* Scan arguments */
  rb_scan_args(__p_argc, __p_argv, "11",&__v_status_code, &__v_reason);

  /* Set defaults */
  __orig_status_code = status_code = NUM2INT(__v_status_code);

  if (__p_argc > 1)
    __orig_reason = reason = ( NIL_P(__v_reason) ? NULL : StringValuePtr(__v_reason) );
  else
    reason = NULL;


#line 120 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  if (reason) { soup_message_set_status_full(_self, status_code, reason);
  } else { soup_message_set_status(_self, status_code);
  }
  return self;
}

static VALUE
Message_set_redirect(VALUE self, VALUE __v_status_code, VALUE __v_redirect_uri)
{
  int status_code; int __orig_status_code;
  char * redirect_uri; char * __orig_redirect_uri;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_status_code = status_code = NUM2INT(__v_status_code);
  __orig_redirect_uri = redirect_uri = ( NIL_P(__v_redirect_uri) ? NULL : StringValuePtr(__v_redirect_uri) );

#line 128 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_set_redirect(_self, status_code, redirect_uri);
 
  return self;
}

static VALUE
Message_is_keepalive_query(VALUE self)
{
  VALUE __p_retval = Qnil;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));

#line 132 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  do { __p_retval =  ((soup_message_is_keepalive(_self)) ? Qtrue : Qfalse); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Message_flags_equals(VALUE self, VALUE __v_flags)
{
  MessageFlags flags; MessageFlags __orig_flags;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_flags = flags = flags_ruby_to_MessageFlags((__v_flags));

#line 136 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  soup_message_set_flags(_self, flags);
 
  return __v_flags;
}

static VALUE
Message_flags(VALUE self)
{
  VALUE __p_retval = Qnil;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));

#line 140 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  do { __p_retval = flags_MessageFlags_to_ruby((soup_message_get_flags(_self))); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Message_get_response_header(VALUE self, VALUE __v_name)
{
  VALUE __p_retval = Qnil;
  char * name; char * __orig_name;
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));
  __orig_name = name = ( NIL_P(__v_name) ? NULL : StringValuePtr(__v_name) );

#line 144 "/home/geoff/Projects/soup/ext/soup/soup.cr"
  do { __p_retval =  rb_str_new2(soup_message_headers_get_one(_self->response_headers, name)); goto out; } while(0);
out:
  return __p_retval;
}

static VALUE
Message_each_response_header(VALUE self)
{
  SoupMessage *_self = ((SoupMessage*)RVAL2GOBJ(self));

#line 148 "/home/geoff/Projects/soup/ext/soup/soup.cr"

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

#line 154 "/home/geoff/Projects/soup/ext/soup/soup.cr"

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
  flagsBaseClass = rb_define_class("Flags", rb_cObject);
    rb_define_method(flagsBaseClass, "inspect", rubber_flags_inspect, 0);
    rb_define_method(flagsBaseClass, "to_i", rubber_flags_to_i, 0);
    rb_define_method(flagsBaseClass, "coerce", rubber_flags_coerce, 1);
    rb_define_method(flagsBaseClass, "to_s", rubber_flags_to_s, 0);
    rb_define_method(flagsBaseClass, "to_str", rubber_flags_to_s, 0);
    rb_define_method(flagsBaseClass, "fullname", rubber_flags_to_s, 0);
    rb_define_method(flagsBaseClass, "name", rubber_flags_name, 0);
    rb_define_method(flagsBaseClass, "<=>", rubber_flags_cmp, 0);
    rb_define_method(flagsBaseClass, "&", rubber_flags_and, 1);
    rb_define_method(flagsBaseClass, "|", rubber_flags_or, 1);
    
  flagsMessageFlags = rb_define_class_under(mSoup, "MessageFlags", flagsBaseClass);
    rb_define_method(flagsMessageFlags, "inspect", rubber_flagsMessageFlags_flags_inspect, 0);
    flagsMessageFlags_SOUP_MESSAGE_NO_REDIRECT = make_flags_value(flagsMessageFlags, SOUP_MESSAGE_NO_REDIRECT, "no-redirect", "SOUP_MESSAGE_NO_REDIRECT");
    rb_obj_freeze(flagsMessageFlags_SOUP_MESSAGE_NO_REDIRECT);
    rb_define_const(flagsMessageFlags, "NO_REDIRECT", flagsMessageFlags_SOUP_MESSAGE_NO_REDIRECT);
    flagsMessageFlags_SOUP_MESSAGE_CAN_REBUILD = make_flags_value(flagsMessageFlags, SOUP_MESSAGE_CAN_REBUILD, "can-rebuild", "SOUP_MESSAGE_CAN_REBUILD");
    rb_obj_freeze(flagsMessageFlags_SOUP_MESSAGE_CAN_REBUILD);
    rb_define_const(flagsMessageFlags, "CAN_REBUILD", flagsMessageFlags_SOUP_MESSAGE_CAN_REBUILD);
    flagsMessageFlags_SOUP_MESSAGE_CONTENT_DECODED = make_flags_value(flagsMessageFlags, SOUP_MESSAGE_CONTENT_DECODED, "content-decoded", "SOUP_MESSAGE_CONTENT_DECODED");
    rb_obj_freeze(flagsMessageFlags_SOUP_MESSAGE_CONTENT_DECODED);
    rb_define_const(flagsMessageFlags, "CONTENT_DECODED", flagsMessageFlags_SOUP_MESSAGE_CONTENT_DECODED);
    flagsMessageFlags_SOUP_MESSAGE_CERTIFICATE_TRUSTED = make_flags_value(flagsMessageFlags, SOUP_MESSAGE_CERTIFICATE_TRUSTED, "certificate-trusted", "SOUP_MESSAGE_CERTIFICATE_TRUSTED");
    rb_obj_freeze(flagsMessageFlags_SOUP_MESSAGE_CERTIFICATE_TRUSTED);
    rb_define_const(flagsMessageFlags, "CERTIFICATE_TRUSTED", flagsMessageFlags_SOUP_MESSAGE_CERTIFICATE_TRUSTED);
    flagsMessageFlags_SOUP_MESSAGE_NEW_CONNECTION = make_flags_value(flagsMessageFlags, SOUP_MESSAGE_NEW_CONNECTION, "new-connection", "SOUP_MESSAGE_NEW_CONNECTION");
    rb_obj_freeze(flagsMessageFlags_SOUP_MESSAGE_NEW_CONNECTION);
    rb_define_const(flagsMessageFlags, "NEW_CONNECTION", flagsMessageFlags_SOUP_MESSAGE_NEW_CONNECTION);
  cMessage = G_DEF_CLASS(SOUP_TYPE_MESSAGE, "Message", mSoup);
  rb_define_method(cMessage, "initialize", Message_initialize, 2);
  rb_define_method(cMessage, "set_request_header", Message_set_request_header, 2);
  rb_define_method(cMessage, "unset_request_header", Message_unset_request_header, 1);
  rb_define_method(cMessage, "set_request_body", Message_set_request_body, 2);
  rb_define_method(cMessage, "set_response", Message_set_response, 2);
  rb_define_method(cMessage, "set_status", Message_set_status, -1);
  rb_define_method(cMessage, "set_redirect", Message_set_redirect, 2);
  rb_define_method(cMessage, "is_keepalive?", Message_is_keepalive_query, 0);
  rb_define_method(cMessage, "flags=", Message_flags_equals, 1);
  rb_define_method(cMessage, "flags", Message_flags, 0);
  rb_define_method(cMessage, "get_response_header", Message_get_response_header, 1);
  rb_define_method(cMessage, "each_response_header", Message_each_response_header, 0);
  rb_define_method(cMessage, "response_body", Message_response_body, 0);
rb_gc_register_address(&_gcpool_RubyFunc);
}
