const binding = require('./binding')

module.exports = exports = class Bitarray {
  constructor() {
    this._allocations = []
    this._handle = Buffer.from(binding.init(this, this._onalloc, this._onfree))
    this._view = new Uint32Array(this._handle, 0, 2)
  }

  _onalloc(size) {
    const buffer = new Uint32Array(size / 4)

    buffer[0] = this._allocations.push(buffer) - 1

    return buffer
  }

  _onfree(id) {
    const last = this._allocations.pop()

    if (last[0] !== id) {
      this._allocations[(last[0] = id)] = last
    }
  }

  page(i) {
    if (typeof i !== 'number') {
      throw new TypeError(
        `\`i\` must be a number, received type ${typeof i} (${i})`
      )
    }

    const id = binding.page(this._handle, i)

    if (id < 0) return null

    const allocation = this._allocations[id]

    return allocation.subarray(binding.constants.PAGE_BITFIELD_OFFSET / 4 + 1)
  }

  *pages() {
    const n = this._view[1] + 1
    if (n === 2 ** 32) return

    for (let i = 0; i < n; i++) {
      const page = this.page(i)

      if (page) yield [i, page]
    }
  }

  insert(bitfield, start = 0) {
    if (typeof start !== 'number') {
      throw new TypeError(
        `\`start\` must be a number, received type ${typeof start} (${start})`
      )
    }

    if (start % 8 !== 0) {
      throw new RangeError('`start` must be a multiple of 8')
    }

    binding.insert(this._handle, bitfield, start)
  }

  clear(bitfield, start = 0) {
    if (typeof start !== 'number') {
      throw new TypeError(
        `\`start\` must be a number, received type ${typeof start} (${start})`
      )
    }

    if (start % 8 !== 0) {
      throw new RangeError('`start` must be a multiple of 8')
    }

    binding.clear(this._handle, bitfield, start)
  }

  get(bit) {
    if (typeof bit !== 'number') {
      throw new TypeError(
        `\`bit\` must be a number, received type ${typeof bit} (${bit})`
      )
    }

    return binding.get(this._handle, bit)
  }

  set(bit, value = true) {
    if (typeof bit !== 'number') {
      throw new TypeError(
        `\`bit\` must be a number, received type ${typeof bit} (${bit})`
      )
    }

    if (typeof value !== 'boolean') {
      throw new TypeError(
        `\`value\` must be a boolean, received type ${typeof value} (${value})`
      )
    }

    if (this.get(bit) === value) return false

    return binding.set(this._handle, bit, value)
  }

  setBatch(bits, value = true) {
    if (!Array.isArray(bits)) {
      throw new TypeError(
        `\`bits\` must be an array, received type ${typeof bits} (${bits})`
      )
    }

    if (typeof value !== 'boolean') {
      throw new TypeError(
        `\`value\` must be a boolean, received type ${typeof value} (${value})`
      )
    }

    return binding.setBatch(this._handle, bits, value)
  }

  unset(bit) {
    return this.set(bit, false)
  }

  unsetBatch(bits) {
    return this.setBatch(bits, false)
  }

  fill(value, start = 0, end = -1) {
    if (typeof value !== 'boolean') {
      throw new TypeError(
        `\`value\` must be a boolean, received type ${typeof value} (${value})`
      )
    }

    if (typeof start !== 'number') {
      throw new TypeError(
        `\`start\` must be a number, received type ${typeof start} (${start})`
      )
    }

    if (typeof end !== 'number') {
      throw new TypeError(
        `\`end\` must be a number, received type ${typeof end} (${end})`
      )
    }

    binding.fill(this._handle, value, start, end)
  }

  findFirst(value, pos = 0) {
    if (typeof value !== 'boolean') {
      throw new TypeError(
        `\`value\` must be a boolean, received type ${typeof value} (${value})`
      )
    }

    if (typeof pos !== 'number') {
      throw new TypeError(
        `\`pos\` must be a number, received type ${typeof pos} (${pos})`
      )
    }

    return binding.findFirst(this._handle, value, pos)
  }

  firstSet(pos) {
    return this.findFirst(true, pos)
  }

  firstUnset(pos) {
    return this.findFirst(false, pos)
  }

  findLast(value, pos = -1) {
    if (typeof value !== 'boolean') {
      throw new TypeError(
        `\`value\` must be a boolean, received type ${typeof value} (${value})`
      )
    }

    if (typeof pos !== 'number') {
      throw new TypeError(
        `\`pos\` must be a number, received type ${typeof pos} (${pos})`
      )
    }

    return binding.findLast(this._handle, value, pos)
  }

  lastSet(pos) {
    return this.findLast(true, pos)
  }

  lastUnset(pos) {
    return this.findLast(false, pos)
  }

  count(value, start = 0, end = -1) {
    if (typeof value !== 'boolean') {
      throw new TypeError(
        `\`value\` must be a boolean, received type ${typeof value} (${value})`
      )
    }

    if (typeof start !== 'number') {
      throw new TypeError(
        `\`start\` must be a number, received type ${typeof start} (${start})`
      )
    }

    if (typeof end !== 'number') {
      throw new TypeError(
        `\`end\` must be a number, received type ${typeof end} (${end})`
      )
    }

    return binding.count(this._handle, value, start, end)
  }

  countSet(start, end) {
    return this.count(true, start, end)
  }

  countUnset(start, end) {
    return this.count(false, start, end)
  }
}

exports.constants = {
  BYTES_PER_PAGE: binding.constants.BYTES_PER_PAGE
}
