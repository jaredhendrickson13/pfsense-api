# Working with Object IDs

## Object `id` field

Object IDs are used to uniquely identify objects in the REST API and are most commonly used to retrieve, update, or
delete objects. Object IDs are included in the object's response data. It is important to note that pfSense does not
use persistent object IDs, meaning that the object ID can change if objects are reordered or deleted. This is because
pfSense uses the object's array index as the object ID instead of a persistent, stored value. It may be necessary to
re-query the object list to get the updated object ID after an object is deleted or reordered to ensure you are working
with the correct object. For example, if you have 3 static route objects stored (IDs 0, 1, and 2) and you delete the
object with ID 1, pfSense will resort the array so the object with ID 2 will now have an ID of 1.

!!! Note
A small number of endpoints do use persistent object IDs. These endpoints will have strings as the object ID instead
of integers and typically relate to a pfSense interface ID (i.e. `wan`, `lan`, `opt1`, etc.).

## Object `parent_id` field

Some objects have a `parent_id` field that is used to identify the parent object of the current object. This field is
used to create a parent-child relationship between objects. For example, a DNS resolver host override alias object will
have a `parent_id` field that references the DNS resolver host override object that the alias is associated with. This
field is required for managing objects that are nested within other objects.

Consider the following example:

```json
{
  "code": 200,
  "status": "ok",
  "response_id": "SUCCESS",
  "message": "",
  "data": {
    "id": 0,
    "host": "parent",
    "domain": "example.com",
    "ip": ["1.2.3.4"],
    "descr": "I am a parent Host Override object.",
    "aliases": [
      {
        "parent_id": 0,
        "id": 0,
        "host": "child1",
        "domain": "example.com",
        "descr": "I am a child Host Override Alias object under my parent Host Override 'parent.example.com'"
      },
      {
        "parent_id": 0,
        "id": 1,
        "host": "child2",
        "domain": "example.com",
        "descr": "I am also a child Host Override Alias object under my parent Host Override 'parent.example.com'"
      }
    ]
  }
}
```

Note how each child object under the `aliases` array has a `parent_id` field that references the parent object's `id`
value. Each child object also contains its own `id` field that can be used to uniquely identify the child object. In
order to correctly obtain a child object, the `parent_id` is required first to look up the parent object. Then the child's `id` is used to
look up the child object from within the parent object.

!!! Important
Like the `id` field, the `parent_id` field is not persistent and can change if objects are reordered or deleted.
