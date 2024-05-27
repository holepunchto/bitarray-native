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
    if (typeof bit !== 'number') {
      throw new TypeError(`\`bit\` must be a number, received type ${typeof bit} (${bit})`)
    }

    return binding.get(this._handle, bit)
  }

  set (bit, value) {
    if (typeof bit !== 'number') {
      throw new TypeError(`\`bit\` must be a number, received type ${typeof bit} (${bit})`)
    }

    if (typeof value !== 'boolean') {
      throw new TypeError(`\`value\` must be a boolean, received type ${typeof value} (${value})`)
    }

    return binding.set(this._handle, bit, value)
  }

  fill (value, start, end) {
    if (typeof value !== 'boolean') {
      throw new TypeError(`\`value\` must be a boolean, received type ${typeof value} (${value})`)
    }

    if (typeof start !== 'number') {
      throw new TypeError(`\`start\` must be a number, received type ${typeof start} (${start})`)
    }

    if (typeof end !== 'number') {
      throw new TypeError(`\`end\` must be a number, received type ${typeof end} (${end})`)
    }

    binding.fill(this._handle, value, start, end)
  }

  findFirst (value, pos = 0) {
    if (typeof value !== 'boolean') {
      throw new TypeError(`\`value\` must be a boolean, received type ${typeof value} (${value})`)
    }

    if (typeof pos !== 'number') {
      throw new TypeError(`\`pos\` must be a number, received type ${typeof pos} (${pos})`)
    }

    return binding.findFirst(this._handle, value, pos)
  }

  firstSet (pos) {
    return this.findFirst(true, pos)
  }

  firstUnset (pos) {
    return this.findFirst(false, pos)
  }

  findLast (value, pos = -1) {
    if (typeof value !== 'boolean') {
      throw new TypeError(`\`value\` must be a boolean, received type ${typeof value} (${value})`)
    }

    if (typeof pos !== 'number') {
      throw new TypeError(`\`pos\` must be a number, received type ${typeof pos} (${pos})`)
    }

    return binding.findLast(this._handle, value, pos)
  }

  lastSet (pos) {
    return this.findLast(true, pos)
  }

  lastUnset (pos) {
    return this.findLast(false, pos)
  }

  count (value, start, end) {
    if (typeof value !== 'boolean') {
      throw new TypeError(`\`value\` must be a boolean, received type ${typeof value} (${value})`)
    }

    if (typeof start !== 'number') {
      throw new TypeError(`\`start\` must be a number, received type ${typeof start} (${start})`)
    }

    if (typeof end !== 'number') {
      throw new TypeError(`\`end\` must be a number, received type ${typeof end} (${end})`)
    }

    return binding.count(this._handle, value, start, end)
  }

  countSet (start, end) {
    return this.count(true, start, end)
  }

  countUnset (start, end) {
    return this.count(false, start, end)
  }

  destroy () {
    if (this._handle === null) return

    binding.destroy(this._handle)

    this._handle = null
  }
}
