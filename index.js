const binding = require('./binding')

module.exports = class Bitarray {
  constructor () {
    this._allocations = []

    this._handle = binding.init(this, this._onalloc, this._onfree)
  }

  _onalloc (size) {
    const buffer = new Uint32Array(size / 4)

    buffer[0] = this._allocations.push(buffer) - 1

    return buffer
  }

  _onfree (id) {
    const last = this._allocations.pop()

    if (last[0] !== id) {
      this._allocations[last[0] = id] = last
    }
  }

  get (bit) {
    return binding.get(this._handle, bit)
  }

  set (bit, value) {
    return binding.set(this._handle, bit, value)
  }

  fill (value, start, end) {
    binding.fill(this._handle, value, start, end)
  }

  findFirst (value, pos = 0) {
    return binding.findFirst(this._handle, value, pos)
  }

  findLast (value, pos = -1) {
    return binding.findLast(this._handle, value, pos)
  }

  count (value, start, end) {
    return binding.count(this._handle, value, start, end)
  }

  destroy () {
    if (this._handle === null) return

    binding.destroy(this._handle)

    this._handle = null
  }
}
