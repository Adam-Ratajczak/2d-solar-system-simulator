# class `World`

The `World` class represents a world. It contains all objects (planets) that were added.

Attributes:
* [`offset : Vec3`](#offset--vec3)
* [`fov : float`](#fov--float)
* [`yaw : float`](#yaw--float)
* [`pitch : float`](#pitch--float)
* [`zoom : float`](#zoom--float)
* [`focused_object : Object`](#focusedobject-object)

Methods:
* [`reset()`](#reset)

## Attributes

### `offset : Vec3`

The offset from the center of coordinate system, in AU

### `fov : float`

Field of view

### `yaw : float`
### `pitch : float`

### `zoom : float`

The view scale. Greater the value is, more you can see (objects look smaller).

### `focused_object: Object`

The currently focused [Object](./Object.md).

## Methods

### `reset() -> None`

Reset view transform to default values:
* `offset` = Vec3(0, 0, 0)
* `yaw` = 0
* `pitch` = 0.7
* `zoom` = 1
