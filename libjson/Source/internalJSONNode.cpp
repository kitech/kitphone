#include "internalJSONNode.h"
#include "NumberToString.h"  //So that I can convert numbers into strings
#include "JSONNode.h"  //To fill in the foreward declaration
#include "JSONWorker.h"  //For fetching and parsing and such

/*
    The point of these constants is for faster assigning, if I
    were to constantly assign to a string literal, there would be 
    lots of copies, but since strings are copy on write, this assignment
    is much faster
*/
static const json_string CONST_TRUE(JSON_TEXT("true"));
static const json_string CONST_FALSE(JSON_TEXT("false"));
static const json_string CONST_NULL(JSON_TEXT("null"));

#ifdef JSON_UNIT_TEST
    void internalJSONNode::incinternalAllocCount(void) json_nothrow { JSONNode::incinternalAllocCount(); }
    void internalJSONNode::decinternalAllocCount(void) json_nothrow { JSONNode::decinternalAllocCount(); }
#endif

internalJSONNode::internalJSONNode(const internalJSONNode & orig) json_nothrow :
    _type(orig._type), _name(orig._name), _name_encoded(orig._name_encoded),
    _string(orig._string), _string_encoded(orig._string_encoded), _value(orig._value)
    initializeMutex(0)
    initializeRefCount(1)
    initializeFetch(orig.fetched)
    initializeComment(orig._comment)
    initializeChildren(0){
	   
	   
    incinternalAllocCount();
    if (isContainer()){
	   CHILDREN = jsonChildren::newChildren();
	   if (json_likely(!orig.CHILDREN -> empty())){
		  CHILDREN -> reserve(orig.CHILDREN -> size());
		  json_foreach(orig.CHILDREN, myrunner){
			 CHILDREN -> push_back(JSONNode::newJSONNode((*myrunner) -> duplicate()));
		  }
	   }
    }
    #ifdef JSON_MUTEX_CALLBACKS
	   _set_mutex(orig.mylock, false);  
    #endif
}

#ifdef JSON_PREPARSE
    #define SetFetchedFalseOrDo(code) code
#else
    #define SetFetchedFalseOrDo(code) SetFetched(false)
#endif

//this one is specialized because the root can only be array or node
#ifdef JSON_READ_PRIORITY
internalJSONNode::internalJSONNode(const json_string & unparsed) json_nothrow : _type(), _name(),_name_encoded(false), _string(unparsed), _string_encoded(), _value()
    initializeMutex(0)
    initializeRefCount(1)
    initializeFetch(false)
    initializeComment(EMPTY_JSON_STRING)
    initializeChildren(0){
	   
    incinternalAllocCount();
    switch (unparsed[0]){
	   case JSON_TEXT('{'):  //node
		  _type = JSON_NODE;
		  CHILDREN = jsonChildren::newChildren();
		  #ifdef JSON_PREPARSE
			 FetchNode();
		  #endif
		  break;
	   case JSON_TEXT('['):  //array
		  _type = JSON_ARRAY;
		  CHILDREN = jsonChildren::newChildren();
		  #ifdef JSON_PREPARSE
			 FetchArray();
		  #endif
		  break;
	   default:
		  JSON_FAIL_SAFE(JSON_TEXT("root not starting with either { or ["), Nullify(););
		  break;
    }
}

#ifndef JSON_STRICT
    #define LETTERCASE(x, y)\
	   case JSON_TEXT(x):\
	   case JSON_TEXT(y)
#else
    #define LETTERCASE(x, y)\
	   case JSON_TEXT(x)
#endif

internalJSONNode::internalJSONNode(const json_string & name_t, const json_string & value_t) json_nothrow : _type(), _name_encoded(), _name(JSONWorker::FixString(name_t, NAME_ENCODED)), _string(), _string_encoded(), _value()
    initializeMutex(0)
    initializeRefCount(1)
    initializeFetch(false)
    initializeComment(EMPTY_JSON_STRING)
    initializeChildren(0){
	   
    incinternalAllocCount();
	   
    #ifdef JSON_STRICT
	   JSON_ASSERT_SAFE(!value_t.empty(), JSON_TEXT("empty node"), Nullify(NOTVALID); return;);
    #else
	   if (json_unlikely(value_t.empty())){
		  _type = JSON_NULL;
		  SetFetched(true);
		  return;
	   }
    #endif
    
    _string = value_t;
    #ifdef JSON_LESS_MEMORY
	   JSON_ASSERT(_string.capacity() == _string.length(), JSON_TEXT("_string object too large"));
    #endif
	   
    const json_char firstchar = value_t[0];
    #if defined JSON_DEBUG || defined JSON_SAFE
	   const json_char lastchar = value_t[value_t.length() - 1];
    #endif
    
    switch (firstchar){
        case JSON_TEXT('\"'):  //a json_string literal, still escaped and with leading and trailing quotes
            JSON_ASSERT_SAFE(lastchar == JSON_TEXT('\"'), JSON_TEXT("Unterminated quote"), Nullify(); return;);
            _type = JSON_STRING;
		  SetFetchedFalseOrDo(FetchString());
            break;
        case JSON_TEXT('{'):  //a child node, or set of children
            JSON_ASSERT_SAFE(lastchar == JSON_TEXT('}'), JSON_TEXT("Missing }"), Nullify(); return;);
            _type = JSON_NODE;
		  CHILDREN = jsonChildren::newChildren();
		  SetFetchedFalseOrDo(FetchNode());
            break;
        case JSON_TEXT('['):  //an array
            JSON_ASSERT_SAFE(lastchar == JSON_TEXT(']'), JSON_TEXT("Missing ]"), Nullify(); return;);
            _type = JSON_ARRAY;
		  CHILDREN = jsonChildren::newChildren();
		  SetFetchedFalseOrDo(FetchArray());
            break;
        LETTERCASE('t', 'T'):
            JSON_ASSERT_SAFE(value_t == JSON_TEXT("true"), json_string(json_string(JSON_TEXT("unknown JSON literal: ")) + value_t).c_str(), Nullify(); return;);
            _value._bool = true;
            _type = JSON_BOOL;
		  SetFetched(true);
            break;
        LETTERCASE('f', 'F'):
            JSON_ASSERT_SAFE(value_t == JSON_TEXT("false"), json_string(json_string(JSON_TEXT("unknown JSON literal: ")) + value_t).c_str(), Nullify(); return;);
            _value._bool = false;
            _type = JSON_BOOL;
		  SetFetched(true);
            break;
        LETTERCASE('n', 'N'):
            JSON_ASSERT_SAFE(value_t == JSON_TEXT("null"), json_string(json_string(JSON_TEXT("unknown JSON literal: ")) + value_t).c_str(), Nullify(); return;);
            _type = JSON_NULL;
		  SetFetched(true);
            break;
        default:
            JSON_ASSERT_SAFE(NumberToString::isNumeric(value_t), json_string(json_string(JSON_TEXT("unknown JSON literal: ")) + value_t).c_str(), Nullify(); return;);
		  _type = JSON_NUMBER;
		  SetFetchedFalseOrDo(FetchNumber());
            break;
    }
}
#endif


internalJSONNode::~internalJSONNode(void) json_nothrow {
    decinternalAllocCount();
    #ifdef JSON_MUTEX_CALLBACKS
	   _unset_mutex();  
    #endif
    DELETE_CHILDREN();
}

#ifdef JSON_READ_PRIORITY
    void internalJSONNode::FetchString(void) const json_nothrow {
	   JSON_ASSERT_SAFE(!_string.empty(), JSON_TEXT("JSON json_string type is empty?"), Nullify(); return;);
	   JSON_ASSERT_SAFE(_string[0] == JSON_TEXT('\"'), JSON_TEXT("JSON json_string type doesn't start with a quotation?"), Nullify(); return;);
	   JSON_ASSERT_SAFE(_string[_string.length() - 1] == JSON_TEXT('\"'), JSON_TEXT("JSON json_string type doesn't end with a quotation?"), Nullify(); return;);
	   _string = JSONWorker::FixString(json_string(_string.begin() + 1, _string.end() - 1), STRING_ENCODED);
	   #ifdef JSON_LESS_MEMORY
		  JSON_ASSERT(_string.capacity() == _string.length(), JSON_TEXT("_string object too large 2"));
	   #endif
    }

    void internalJSONNode::FetchNode(void) const json_nothrow {
	   JSON_ASSERT_SAFE(!_string.empty(), JSON_TEXT("JSON node type is empty?"), Nullify(); return;);
	   JSON_ASSERT_SAFE(_string[0] == JSON_TEXT('{'), JSON_TEXT("JSON node type doesn't start with a bracket?"), Nullify(); return;);
	   JSON_ASSERT_SAFE(_string[_string.length() - 1] == JSON_TEXT('}'), JSON_TEXT("JSON node type doesn't end with a bracket?"), Nullify(); return;);
	   JSONWorker::DoNode(this, _string);
	   clearString(_string);
    }

    void internalJSONNode::FetchArray(void) const json_nothrow {
	   JSON_ASSERT_SAFE(!_string.empty(), JSON_TEXT("JSON node type is empty?"), Nullify(); return;);
	   JSON_ASSERT_SAFE(_string[0] == JSON_TEXT('['), JSON_TEXT("JSON node type doesn't start with a square bracket?"), Nullify(); return;);
	   JSON_ASSERT_SAFE(_string[_string.length() - 1] == JSON_TEXT(']'), JSON_TEXT("JSON node type doesn't end with a square bracket?"), Nullify(); return;);
	   JSONWorker::DoArray(this, _string);
	   clearString(_string);
    }

#endif

//This one is used by as_int and as_float, so even non-readers need it
void internalJSONNode::FetchNumber(void) const json_nothrow {
    #if defined(JSON_OCTAL) || defined(JSON_UNICODE)
	   const size_t len = _string.length();
    #endif

    #ifdef JSON_STRICT
	   _value._number = NumberToString::_atof(_string.c_str())
    #else
	   #ifdef JSON_UNICODE
		  json_auto<char> temp(len + 1);
		  wcstombs(temp.ptr, _string.c_str(), len);
		  temp.ptr[len] = '\0';
		  _value._number = (json_number)atof(temp.ptr);
	   #else
		  _value._number = (json_number)atof(_string.c_str());
	   #endif
    #endif
}

#if !defined(JSON_PREPARSE) && defined(JSON_READ_PRIORITY) 
    void internalJSONNode::Fetch(void) const json_nothrow {
	   if (fetched) return;
	   switch (type()){
		  case JSON_STRING:
			 FetchString();
			 break;
		  case JSON_NODE:
			 FetchNode();
			 break;
		  case JSON_ARRAY:
			 FetchArray();
			 break;
		  case JSON_NUMBER:
			 FetchNumber();
			 break;
		  #if defined JSON_DEBUG || defined JSON_SAFE
			 default:
				JSON_FAIL(JSON_TEXT("Fetching an unknown type"));
				Nullify();
		  #endif
	   }
	   fetched = true;  
    }
#endif

void internalJSONNode::Set(const json_string & val) json_nothrow {
    makeNotContainer();
    _type = JSON_STRING;
    _string = shrinkString(val);
    _string_encoded = true;
    SetFetched(true);
}

#ifdef JSON_LIBRARY
    void internalJSONNode::Set(long val) json_nothrow {
	   makeNotContainer();
	   _type = JSON_NUMBER;
	   _value._number = (json_number)val;
	   _string = NumberToString::_itoa<long>(val);
	   SetFetched(true);
    }

    void internalJSONNode::Set(json_number val) json_nothrow {
	   makeNotContainer();
	   _type = JSON_NUMBER;
	   _value._number = val;
	   _string = NumberToString::_ftoa(val);
	   SetFetched(true);
    }
#else
    #define SET(converter, type)\
	   void internalJSONNode::Set(type val) json_nothrow {\
		  makeNotContainer();\
		  _type = JSON_NUMBER;\
		  _value._number = (json_number)val;\
		  _string = NumberToString::converter<type>(val);\
		  SetFetched(true);\
	   }
    #define SET_INTEGER(type) SET(_itoa, type) SET(_uitoa, unsigned type)
    #define SET_FLOAT(type) \
	   void internalJSONNode::Set(type val) json_nothrow {\
		  makeNotContainer();\
		  _type = JSON_NUMBER;\
		  _value._number = (json_number)val;\
		  _string = NumberToString::_ftoa(_value._number);\
		  SetFetched(true);\
	   }

    SET_INTEGER(char)
    SET_INTEGER(short)
    SET_INTEGER(int)
    SET_INTEGER(long)
    #ifndef JSON_ISO_STRICT
	   SET_INTEGER(long long)
    #endif

    SET_FLOAT(float)
    SET_FLOAT(double)
#endif

void internalJSONNode::Set(bool val) json_nothrow {
    makeNotContainer();
    _type = JSON_BOOL;
    _value._bool = val;
    _string = val ? CONST_TRUE : CONST_FALSE;
    SetFetched(true);
}

bool internalJSONNode::IsEqualTo(const internalJSONNode * val) const json_nothrow {
    if (this == val) return true;  //same internal object, so they must be equal (not only for ref counting)
    if (type() != val -> type()) return false;	 //aren't even same type
    if (_name != val -> _name) return false;  //names aren't the same
    if (type() == JSON_NULL) return true;  //both null, can't be different
    #if !defined(JSON_PREPARSE) && defined(JSON_READ_PRIORITY) 
	   Fetch();
	   val -> Fetch();
    #endif
    switch (type()){
	   case JSON_STRING:
		  return val -> _string == _string;
	   case JSON_NUMBER:
		  return _floatsAreEqual(val -> _value._number, _value._number);
	   case JSON_BOOL:
		  return val -> _value._bool == _value._bool;
    };
    
    JSON_ASSERT(type() == JSON_NODE || type() == JSON_ARRAY, JSON_TEXT("Checking for equality, not sure what type"));
    if (CHILDREN -> size() != val -> CHILDREN -> size()) return false;  //if they arne't he same size then they certainly aren't equal
    
    //make sure each children is the same
    JSONNode ** valrunner = val -> CHILDREN -> begin();
    json_foreach(CHILDREN, myrunner){
        JSON_ASSERT(*myrunner, JSON_TEXT("a null pointer within the children"));
	   JSON_ASSERT(*valrunner, JSON_TEXT("a null pointer within the children"));
	   JSON_ASSERT(valrunner != val -> CHILDREN -> end(), JSON_TEXT("at the end of other one's children, but they're the same size?"));
        if (**myrunner != **valrunner) return false;
	   ++valrunner;
    }
    return true;
}

void internalJSONNode::Nullify(void) const json_nothrow {
    _type = JSON_NULL;
    _string = CONST_NULL;
    SetFetched(true);
}
    
#ifdef JSON_MUTEX_CALLBACKS
    #define JSON_MUTEX_COPY ,mylock
#else
    #define JSON_MUTEX_COPY
#endif

#ifdef JSON_LIBRARY
void internalJSONNode::push_back(JSONNode * node) json_nothrow {
#else
void internalJSONNode::push_back(const JSONNode & node) json_nothrow {
#endif
    JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling push_back on non-container type"), return;);
    #ifdef JSON_LIBRARY
	   #ifdef JSON_MUTEX_CALLBACKS
		  if (mylock != 0) node -> set_mutex(mylock);
	   #endif
	   CHILDREN -> push_back(node);
    #else
	   CHILDREN -> push_back(JSONNode::newJSONNode(node   JSON_MUTEX_COPY));
    #endif
}
    
void internalJSONNode::push_front(const JSONNode & node) json_nothrow {
    JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling at on non-container type"), return;);
    CHILDREN -> push_front(JSONNode::newJSONNode(node   JSON_MUTEX_COPY));
}

JSONNode * internalJSONNode::pop_back(json_index_t pos) json_nothrow {
    JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling at on non-container type"), return 0;);
    JSONNode * result = (*CHILDREN)[pos];
    JSONNode ** temp = CHILDREN -> begin() + pos;
    CHILDREN -> erase(temp);
    return result;
}

JSONNode * internalJSONNode::pop_back(const json_string & name_t) json_nothrow {
    JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling pop_back (named) on non-container type"), return 0;);
    if (JSONNode ** res = at(name_t)){
	   JSONNode * result = *res;
	   CHILDREN -> erase(res);
	   return result;
    }
    return 0;
}

#ifdef JSON_CASE_INSENSITIVE_FUNCTIONS
    JSONNode * internalJSONNode::pop_back_nocase(const json_string & name_t) json_nothrow {
	   JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling pop_back_nocase on non-container type"), return 0;);
	   if (JSONNode ** res = at_nocase(name_t)){
		  JSONNode * result = *res;
		  CHILDREN -> erase(res);
		  return result;
	   }
	   return 0;
    }
#endif

JSONNode ** internalJSONNode::at(const json_string & name_t) json_nothrow {
    JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling at on non-container type"), return 0;);
    Fetch();
    json_foreach(CHILDREN, myrunner){
	   JSON_ASSERT(*myrunner, JSON_TEXT("a null pointer within the children"));
	   if (json_unlikely((*myrunner) -> name() == name_t)) return myrunner;
    }
    return 0;
}

#ifdef JSON_CASE_INSENSITIVE_FUNCTIONS
    bool internalJSONNode::AreEqualNoCase(const json_char * ch_one, const json_char * ch_two) json_nothrow {
	   while (*ch_one){  //only need to check one, if the other one terminates early, the check will cause it to fail
		  const json_char c_one = *ch_one;
		  const json_char c_two = *ch_two;
		  if (c_one != c_two){
			 if ((c_two > 64) && (c_two < 91)){  //A - Z
				if (c_one != (json_char)(c_two + 32)) return false;
			 } else if ((c_two > 96) && (c_two < 123)){  //a - z
				if (c_one != (json_char)(c_two - 32)) return false;
			 } else { //not a letter, so return false
				return false;
			 }
		  }
		  ++ch_one;
		  ++ch_two;
		  
	   }
	   return *ch_two == '\0';  //this one has to be null terminated too, or else json_string two is longer, hence, not equal
    }

    JSONNode ** internalJSONNode::at_nocase(const json_string & name_t) json_nothrow {
	   JSON_ASSERT_SAFE(isContainer(), JSON_TEXT("calling at_nocase on non-container type"), return 0;);
	   Fetch();
	   json_foreach(CHILDREN, myrunner){
		  JSON_ASSERT(*myrunner, JSON_TEXT("a null pointer within the children"));
		  if (json_unlikely(AreEqualNoCase((*myrunner) -> name().c_str(), name_t.c_str()))) return myrunner;
	   }
	   return 0;
    }
#endif

#if !defined(JSON_PREPARSE) && defined(JSON_READ_PRIORITY) 
    void internalJSONNode::preparse(void) json_nothrow {
	   Fetch();
	   if (isContainer()){
		  json_foreach(CHILDREN, myrunner){
			 (*myrunner) -> preparse();
		  }
	   }
    }
#endif

#ifdef JSON_DEBUG
    #ifndef JSON_LIBRARY
	   JSONNode internalJSONNode::Dump(size_t & totalbytes) const json_nothrow {
		  JSONNode dumpage(JSON_NODE);
		  dumpage.set_name(JSON_TEXT("internalJSONNode"));
		  dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("this"), (long)this)));
		  
		  START_MEM_SCOPE
			 size_t memory = sizeof(internalJSONNode);
			 memory += _name.capacity() * sizeof(json_char);
			 memory += _string.capacity() * sizeof(json_char);
			 if (isContainer()){
				memory += sizeof(jsonChildren);
				memory += CHILDREN -> capacity() * sizeof(JSONNode*);
			 }
			 #ifdef JSON_COMMENTS
				memory += _comment.capacity() * sizeof(json_char);
			 #endif
			 totalbytes += memory;
			 dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("bytes used"), memory)));
		  END_MEM_SCOPE
		  
		  
		  #ifdef JSON_REF_COUNT
			 dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("refcount"), refcount)));
		  #endif
		  #ifdef JSON_MUTEX_CALLBACKS
			 dumpage.push_back(JSON_NEW(DumpMutex()));
		  #endif


		  #define DUMPCASE(ty)\
			 case ty:\
				dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("_type"), JSON_TEXT(#ty))));\
				break;
		  
		  switch(type()){
			 DUMPCASE(JSON_NULL)
			 DUMPCASE(JSON_STRING)
			 DUMPCASE(JSON_NUMBER)
			 DUMPCASE(JSON_BOOL)
			 DUMPCASE(JSON_ARRAY)
			 DUMPCASE(JSON_NODE)
			 default:
				dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("_type"), JSON_TEXT("Unknown"))));
		  }
		  
		  JSONNode str(JSON_NODE);
		  str.set_name(JSON_TEXT("_name"));
		  str.push_back(JSON_NEW(JSONNode(json_string(JSON_TEXT("value")), _name)));
		  str.push_back(JSON_NEW(JSONNode(JSON_TEXT("length"), _name.length())));
		  str.push_back(JSON_NEW(JSONNode(JSON_TEXT("capactiy"), _name.capacity())));
		  
		  dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("_name_encoded"), _name_encoded)));
		  dumpage.push_back(JSON_NEW(str));
		  dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("_string_encoded"), _string_encoded)));
		  str.clear();
		  str.set_name(JSON_TEXT("_string"));
		  str.push_back(JSON_NEW(JSONNode(json_string(JSON_TEXT("value")), _string)));
		  str.push_back(JSON_NEW(JSONNode(JSON_TEXT("length"), _string.length())));
		  str.push_back(JSON_NEW(JSONNode(JSON_TEXT("capactiy"), _string.capacity())));
		  dumpage.push_back(JSON_NEW(str));
		  
		  if ((type() == JSON_BOOL) || (type() == JSON_NUMBER)){
			 JSONNode unio(JSON_NODE);
			 unio.set_name(JSON_TEXT("_value"));
			 if (type() == JSON_BOOL){
				unio.push_back(JSON_NEW(JSONNode(JSON_TEXT("_bool"), _value._bool)));
			 } else if (type() == JSON_NUMBER){
				unio.push_back(JSON_NEW(JSONNode(JSON_TEXT("_number"), _value._number)));
			 }
			 dumpage.push_back(JSON_NEW(unio));
		  }
		  
		  #if !defined(JSON_PREPARSE) && defined(JSON_READ_PRIORITY) 
			 dumpage.push_back(JSON_NEW(JSONNode(JSON_TEXT("fetched"), fetched)));
		  #endif
		  
		  #ifdef JSON_COMMENTS
			 str.clear();
			 str.set_name(JSON_TEXT("_comment"));
			 str.push_back(JSON_NEW(JSONNode(JSON_TEXT("value"), _comment)));
			 str.push_back(JSON_NEW(JSONNode(JSON_TEXT("length"), _comment.length())));
			 str.push_back(JSON_NEW(JSONNode(JSON_TEXT("capactiy"), _comment.capacity())));
			 dumpage.push_back(JSON_NEW(str));
		  #endif
		  
		  if (isContainer()){
			 JSONNode arra(JSON_NODE);
			 arra.set_name(JSON_TEXT("Children"));
			 arra.push_back(JSON_NEW(JSONNode(JSON_TEXT("size"), CHILDREN -> size())));
			 arra.push_back(JSON_NEW(JSONNode(JSON_TEXT("capacity"), CHILDREN -> capacity())));
			 JSONNode chil(JSON_ARRAY);
			 chil.set_name(JSON_TEXT("array"));
			 json_foreach(CHILDREN, it){
				chil.push_back(JSON_NEW((*it) -> dump(totalbytes)));
			 }
			 arra.push_back(JSON_NEW(chil));
			 dumpage.push_back(JSON_NEW(arra));
		  }
		  
		  return dumpage;
	   }
    #endif
#endif
