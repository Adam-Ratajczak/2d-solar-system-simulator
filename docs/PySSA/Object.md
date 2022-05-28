# class `Object`

The `Object` class represents a single object (planet). 

Attributes:
* [`pos : Vec3`](#pos--vec3)
* [`vel : Vec3`](#vel--vec3)
* [`name : str`](#name--str)
* [`color : ColorRGB`](#color--colorrgb)
* [`radius : float`](#radius--float)

Methods:
* [`attraction(other: Object) -> Vec3`](#attractionother-object---vec3)

## Attributes

### `pos : Vec3`
Object position (in m)

### `vel : Vec3`
Object velocity (in m/tick)

### `name : str`
Object name (to be used with world.get_object_by_name())

### `color : ColorRGB`
Object sphere and trail color

### `radius : float`
Object radius (in meters)

## Methods

### `attraction(other: Object) -> Vec3`

Returns gravity of this object to object given as argument, as acceleration (in m/tick^2)

