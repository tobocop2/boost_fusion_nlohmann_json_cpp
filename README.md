# README

This library defines a bunch of generic to_json and from_json functions to easily serialize and deserialize types in any namespace

It depends on nlohmann/json and a few boost headers. 

To build the example, you either need a full boost installation available or you can clone this repo recursively and invoke the top level Makefile

# Supported container types 

## Sequence types supported:
* array
* vector
* deque
* list
* forward_list

## Ordered Associative containers Supported :
* map **Key must be integral or string**
* set **Key must have comparison operator or will not compile**

## Unordered associative containers Supported :
* unordered_map **Key must be integral or string **
* unordered_set **Key MUST be hashable or will not compile **

## Heterogeneous types supported:
* pair
* tuple 

## Smart pointers supported 
* unique_ptr
* shared_ptr
* 


# Unsupported Types with no plans to be supported

## Following Smart pointer types not supported
* weak_ptr // requires a smart pointer to be instantiated 
* auto_ptr // will be deprecated so no plan for this

## Primitive pointer / array types not supported
* C arrays
* C style pointers


## Container adaptors not yet supported and not planned to be supported:
* stack
* queue
* priority_queue

## Ordered Associative containers not planned to be supported  
* multiset ** no translation to json **

## Unordered Associative containers not supported and not planned to be supported:
* unordered_multiset ** no translation to json **
* unordered_multimap ** no translation to json **
