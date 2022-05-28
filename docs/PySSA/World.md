# class `World`

The `World` class represents a world. It contains all objects (planets) that were added.

Attributes:
* [`simulation_seconds_per_tick : int`](#simulationsecondspertick--int)

Methods:
* [`add_object(object: Object) -> None`](#addobjectobject---none)
* [`get_object_by_name() -> Object`](#getobjectbyname---object)

## Attributes

### `simulation_seconds_per_tick : int`

How much simulation seconds passes per tick. This directly controls simulation accuracy.

## Methods

### `add_object(object: Object) -> None`

Adds an [object](./Object.md) to the World.

### `get_object_by_name() -> Object`

Returns an [object](./Object.md) that has the name given in argument.
