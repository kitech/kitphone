#include "JSONMemory.h"

#ifdef JSON_MEMORY_MANAGE
    #include "JSONNode.h"
    void auto_expand::purge(void) json_nothrow {
	   for(JSON_MAP(void *, void *)::iterator i = mymap.begin(), en = mymap.end(); i != en; ++i){
		  #if defined(JSON_DEBUG) || defined(JSON_SAFE)
			 void * temp = (void*)i -> first;  //because its pass by reference
			 libjson_free<void>(temp);
		  #else
			 libjson_free<void>((void*)i -> first);
		  #endif
	   }
    }

    void auto_expand_node::purge(void) json_nothrow {
	   for(JSON_MAP(void *, JSONNode *)::iterator i = mymap.begin(), en = mymap.end(); i != en; ++i){
		  JSONNode::deleteJSONNode((JSONNode *)i -> second);
	   }
    }

    #ifdef JSON_STREAM
	   #include "JSONStream.h"
	   void auto_expand_stream::purge(void) json_nothrow {
		  for(JSON_MAP(void *, JSONStream *)::iterator i = mymap.begin(), en = mymap.end(); i != en; ++i){
			 JSONStream::deleteJSONStream((JSONStream *)i -> second);
		  }
	   }
    #endif
#endif

#ifdef JSON_MEMORY_CALLBACKS

#include "JSONSingleton.h"

void * JSONMemory::json_malloc(size_t siz) json_nothrow {
    if (json_malloc_t callback = JSONSingleton<json_malloc_t>::get()){
	   #if(defined(JSON_DEBUG) && (!defined(JSON_MEMORY_CALLBACKS))) //in debug mode without mem callback, see if the malloc was successful
		  void * result = callback(siz);
		  JSON_ASSERT(result, JSON_TEXT("Out of memory"));
		  return result;
	   #else
		  return callback(siz);
	   #endif
    }
    #if(defined(JSON_DEBUG) && (!defined(JSON_MEMORY_CALLBACKS))) //in debug mode without mem callback, see if the malloc was successful
	   void * result = std::malloc(siz);
	   JSON_ASSERT(result, JSON_TEXT("Out of memory"));
	   return result;
    #else
	   return std::malloc(siz);
    #endif
}

void * JSONMemory::json_realloc(void * ptr, size_t siz) json_nothrow {
    if (json_realloc_t callback = JSONSingleton<json_realloc_t>::get()){
    #if(defined(JSON_DEBUG) && (!defined(JSON_MEMORY_CALLBACKS))) //in debug mode without mem callback, see if the malloc was successful
		  void * result = callback(ptr, siz);
		  JSON_ASSERT(result, JSON_TEXT("Out of memory"));
		  return result;
	   #else
		  return callback(ptr, siz);
	   #endif
    }
    #if(defined(JSON_DEBUG) && (!defined(JSON_MEMORY_CALLBACKS))) //in debug mode without mem callback, see if the malloc was successful
	   void * result = std::realloc(ptr, siz);
	   JSON_ASSERT(result, JSON_TEXT("Out of memory"));
	   return result;
    #else
	   return std::realloc(ptr, siz);
    #endif
}

void JSONMemory::json_free(void * ptr) json_nothrow {
    if (json_free_t callback = JSONSingleton<json_free_t>::get()){
	   callback(ptr);
    } else {
	   std::free(ptr);
    }
}

void JSONMemory::registerMemoryCallbacks(json_malloc_t mal, json_realloc_t real, json_free_t fre) json_nothrow {
    JSONSingleton<json_malloc_t>::set(mal);
    JSONSingleton<json_realloc_t>::set(real);
    JSONSingleton<json_free_t>::set(fre);
}

#endif
