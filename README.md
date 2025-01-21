# bitarray-native

<https://github.com/holepunchto/libbitarray> bindings for JavaScript.

```
npm i bitarray-native
```

## Usage

```js
const Bitarray = require('bitarray-native')

const b = new Bitarray()

b.set(1234, true)
b.get(1234)
// true
```

## API

#### `const bitarray = new Bitarray()`

#### `bitarray.destroy()`

#### `bitarray.page(index, bitfield)`

#### `bitarray.insert(bitfield[, start])`

#### `bitarray.clear(bitfield[, start])`

#### `bitarray.get(bit)`

#### `bitarray.set(bit[, value])`

#### `bitarray.setBatch(bits[, value])`

#### `bitarray.unset(bit)`

#### `bitarray.unsetBatch(bits)`

#### `bitarray.fill(value[, start[, end]])`

#### `bitarray.findFirst(value[, position])`

#### `bitarray.firstSet([position])`

#### `bitarray.firstUnsetSet([position])`

#### `bitarray.findLast(value[, position])`

#### `bitarray.lastSet([position])`

#### `bitarray.lastUnset([position])`

#### `bitarray.count(value[, start[, end]])`

#### `bitarray.countSet([start[, end]])`

#### `bitarray.countUnset([start[, end]])`

## License

Apache-2.0
