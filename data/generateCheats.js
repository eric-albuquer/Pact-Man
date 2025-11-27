const keys = "WASD QIKLJUO";

for (let i = 1; i <= 16; i++) {
    let str = ""
    for (let j = 0; j < 10; j++) {
        const idx = Math.floor(Math.random() * keys.length)
        str += keys[idx]
    }
    console.log(str + String.fromCharCode(i + 48))
}