
/**
 *  _____  _____  ___  ______ _____
 * /  ___||  _  |/ _ \ | ___ \  ___|
 * \ `--. | | | / /_\ \| |_/ / |__
 *  `--. \| | | |  _  ||    /|  __|
 * /\__/ /\ \_/ / | | || |\ \| |___
 * \____/  \___/\_| |_/\_| \_\____/
 *
 * Antoine LANDRIEUX (MIT License) <SoareMinifier.js>
 * <https://github.com/AntoineLandrieux/SOARE/>
 *
 */

/**
 * Soare Minifier
 */
class Minifier {

    MaxCharPerLine = 100;
    #Tokens = [];

    /**
     * Soare Minifier
     * @author Antoine Landrieux
     * 
     * @param {string} rawcode 
     */
    constructor(rawcode) {
        this.rawcode = rawcode;
    }

    /**
     * Transform a string into a sequence of tokens
     * @author Antoine Landrieux
     * 
     * @returns {string[]}
     */
    Tokenizer() {

        this.#Tokens = [];
        let index = 0;

        while (index < this.rawcode.length) {

            let adder = 1;

            if (/\s/.test(this.rawcode[index])) {
                index += adder;
                continue;
            }

            if (this.rawcode[index] === '?') {
                while (!/\n|\r/.test(this.rawcode[index + adder]) && this.rawcode[index + adder]) adder++;
                index += adder;
                continue;
            }

            if ('$@,;:[]()<,+-^*/%>&|!='.includes(this.rawcode[index]));

            else if (/[a-zA-Z_]/.test(this.rawcode[index])) {
                while (/[a-zA-Z0-9_]/.test(this.rawcode[index + adder]) && this.rawcode[index + adder])
                    adder++;
            }

            else if (/[0-9]/.test(this.rawcode[index])) {
                while (!isNaN(this.rawcode.slice(index, index + adder + 1)) && !/\s/.test(this.rawcode[index + adder]) && this.rawcode[index + adder])
                    adder++;
            }

            else if (/["'`]/.test(this.rawcode[index])) {
                const quote = this.rawcode[index];
                while (this.rawcode[index + adder] != quote && this.rawcode[index + adder])
                    adder++;
                adder++;
            }

            else {
                throw new Error("CharacterError");
            }

            this.#Tokens.push(this.rawcode.slice(index, index + adder));
            index += adder;
        }

        return this.#Tokens;
    }

    /**
     * Apply modification
     * @author Antoine Landrieux
     * 
     * @returns {string}
     */
    Apply() {

        let code = "";
        let ln = 1;

        for (let tkn = 0; tkn < this.#Tokens.length; tkn++) {
            if (code.length >= this.MaxCharPerLine * ln) {
                ln++;
                code += '\n';
            }

            if (/[a-zA-Z0-9_]{1,}/.test(this.#Tokens[tkn]) && /[a-zA-Z0-9_]{1,}/.test(this.#Tokens[tkn + 1]))
                code += `${this.#Tokens[tkn]} `;
            else
                code += this.#Tokens[tkn];
        }

        return code;

    }
}

//
// Example:
//
// const fs = require("fs");
//
// fs.readFile('script/std.soare', 'utf-8', (err, data) => {
//     if (err)
//         throw err;
//     let min = new Minifier(data);
//     min.MaxCharPerLine = 150;
//     min.Tokenizer();
//     console.log(min.Apply());
// });
//
