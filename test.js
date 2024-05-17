const test = require('brittle')
const Bitarray = require('.')

test('basic', (t) => {
  const b = new Bitarray()

  t.is(b.get(100000), false)
  t.is(b.set(100000, true), true)
  t.is(b.get(100000), true)
})
