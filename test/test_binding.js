const Indi = require("../dist/binding.js");
const assert = require("assert");

assert(Indi, "The expected module is undefined");

function testBasic()
{
    const instance = new Indi();
    assert(instance.greet, "The expected method is not defined");
    assert.strictEqual(instance.greet(function(err, name) { console.log(name ? name.getDeviceName() : "unknow") }), "bla", "Unexpected value returned");
}

function testInvalidParams()
{
    const instance = new Indi("bla");
}

assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
//assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");
