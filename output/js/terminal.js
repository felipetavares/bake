term = {
	"const": {
		"BEL": 0x07,
		"BS": 0x08,
		"HT": 0x09,
		"LF": 0x0A,
		"VT": 0x0B,
		"FF": 0x0C,
		"CR": 0x0D,
		"SO": 0x0E,
		"SI": 0x0F,
		"CAN": 0x18,
		"SUB": 0x1A,
		"ESC": 0x1B,
		"DEL": 0x7F,
		"CSI": 0x9B
	},

	"vec2": function (x, y) {
		this.x = x;
		this.y = y;
	},

	"style": function () {
		this.bg = 0;
		this.fg = 7;
		this.weight = 0;
		this.rev = 0;

		this.copy = function () {
			var ns = new term.style();
			if (this.rev) {
				ns.bg = this.fg;
				ns.fg = this.bg;
			} else {
				ns.bg = this.bg;
				ns.fg = this.fg;
			}
			ns.weight = this.weight;
			return ns;
		}
	},

	"Terminal": function (w, h, id) {
		this.size = new term.vec2(w, h);
		this.cursor = new term.vec2(0, 0);
		this.buffer = new Array(w*h);
		this.styleBuffer = new Array(w*h);
		this.html = "";
		this.element = document.getElementById(id);
		this.style = new term.style();
		this.mode = 0;

		this.getc = function (p) {
			return this.buffer[this.size.x*p.y+p.x];
		}

		this.gets = function (p) {
			return this.styleBuffer[this.size.x*p.y+p.x];
		}

		this.putc = function (p, c) {
			this.buffer[this.size.x*p.y+p.x] = c;
			this.styleBuffer[this.size.x*p.y+p.x] = this.style.copy();
		}

		this.advance = function () {
			this.cursor.x ++;
		}

		this.render = function () {
			var size = this.size.x*this.size.y;
			this.html = "";
			for (var y=0;y<this.size.y;y++) {
				for (var x=0;x<this.size.x;x++) {
					var p = new term.vec2(x, y);
					var c = this.getc(p);
					var s = this.gets(p);

					if (c === 0) {
						this.html += ' ';
					} else if (c == ' ' && s.bg == 0) {
						this.html += c;
					} else {
						var colors = [
							"#000",
							"#C23621",
							"#25BC24",
							"#ADAD27",
							"#492EFF",
							"#D338D3",
							"#33BBC8",
							"#CBCCCD",
						];
						var weights = [
							"normal",
							"bold"
						];
						if (s.bg != 0 || s.fg != 7 || s.weight != 0) {
							this.html += "<span style='font-weight: "+weights[s.weight]+";background: "+colors[s.bg]+"; color: "+colors[s.fg]+";'>"+c+"</span>";
						} else {
							this.html += c;
						}
					}
				}
				this.html += "<br/>";
			}

			this.element.innerHTML = this.html;
		}

		this.eraseDisplay = function (s, e) {
			while (s <= e) {
				this.buffer[s++] = 0;
			}
		}

		this.eraseLine = function (s, e) {
			while (s <= e) {
				this.putc(new term.vec2(s++, this.cursor.y), 0);
			}
		}

		this.curArg = 0;
		this.args = [];
		this.input = function (c) {
			switch (this.mode) {
				case 0:
				switch (c) {
					case term.const.BEL:
						// BEEP
					break;
					case term.const.BS:
						if (this.cursor.x > 0)
							this.cursor.x --;
					break;
					case term.const.HT:
						// Next tab stop (what does this means?)
					break;
					case term.const.LF: case term.const.VT: case term.const.FF:
						if (this.cursor.y < this.size.y-1)
							this.cursor.y ++;
					break;
					case term.const.CR:
						this.cursor.x = 0;
					break;
					case term.const.SO:
						// G1 Charset
					break;
					case term.const.SI:
						// G0 Charset
					break;
					case term.const.CAN: case term.const.SUB:
						// Interrupt Escape Seq
					break;
					case term.const.ESC:
						this.mode = 1;
					break;
					case term.const.CSI:
						// Begin escape seq
						this.args = [];
						this.mode = 2;
					break;
					case term.const.DEL:
						// Ignore
					break;
					default:
						this.putc(this.cursor, String.fromCharCode(c));
						this.advance();
				}
				break;
				case 1:
					if (String.fromCharCode(c) == '[') {
						this.args = [];
						this.mode = 2;
					} else {
						console.log(String.fromCharCode(c));
						this.mode = 0;
					}
				break;
				case 2:
					var str = String.fromCharCode(c);
					if ((str >= '0' && str <= '9')) {
						if (this.args.length == 0) {
							this.curArg = 0;
							this.args=[""];
						}
						this.args[this.curArg] += str;
					} else if (str == ';') {
						this.curArg++;
						this.args.push("");
					} else {
						switch (str) {
							case 'H':
								this.cursor.y = this.args[0]==undefined?0:Number.parseInt(this.args[0])-1;
								this.cursor.x = this.args[1]==undefined?0:Number.parseInt(this.args[1])-1;

								if (this.cursor.y < 0)
									this.cursor.y=0;
								if (this.cursor.x < 0)
									this.cursor.x=0;
							break;
							case 'A':
								this.cursor.y -= this.args[0]==undefined?1:Number.parseInt(this.args[0]);
								if (this.cursor.y < 0)
									this.cursor.y=0;
							break;
							case 'B':
								this.cursor.y += this.args[0]==undefined?1:Number.parseInt(this.args[0]);
								if (this.cursor.y < 0)
									this.cursor.y=0;
							break;
							case 'C':
								this.cursor.x += this.args[0]==undefined?1:Number.parseInt(this.args[0]);
								if (this.cursor.x < 0)
									this.cursor.x=0;
							break;
							case 'D':
								this.cursor.x -= this.args[0]==undefined?1:Number.parseInt(this.args[0]);
								if (this.cursor.x < 0)
									this.cursor.x=0;
							break;
							case 'K':
								if (this.args.length) {
									if (this.args[0] == "1") {
										this.eraseLine(0, this.cursor.x);
									} else {
										this.eraseLine(0, this.size.x-1);
									}
								} else {
									this.eraseLine(this.cursor.x, this.size.x-1);
								}
							break;
							case 'J':
								if (this.args.length) {
									if (this.args[0] == "1") {
										this.eraseDisplay(0, this.cursor.x+this.cursor.y*this.size.x);
									} else {
										this.eraseDisplay(0, this.size.x*this.size.y-1);
									}
								} else {
									this.eraseDisplay(this.cursor.x+this.cursor.y*this.size.x, this.size.x*this.size.y-1);
								}
							break;
							case 'm':
								for (a in this.args) {
									if (this.args[a] == "0") {
										this.style = new term.style();
									} else
									if (this.args[a][0] == '3') {
										if (this.args[a][1] == '8' || this.args[a][1] == '9')
											this.style.fg = 7;
										else
											this.style.fg = Number.parseInt(this.args[a][1]);
									} else
									if (this.args[a][0] == '4') {
										if (this.args[a][1] == '9')
											this.style.bg = 0;
										else
											this.style.bg = Number.parseInt(this.args[a][1]);
									} else
									if (this.args[a] == "1") {
										this.style.weight = 1;
									} else
									if (this.args[a] == "7") {
										this.style.rev = 1;
									} else {
										console.log(this.args)
									}
								}
							break;
							default:
								console.log(str);
						}
						this.mode = 0;
					}
				break;
			}
		}

		// @constructor
		var size = w*h;
		for (var i=0;i<size;i++) {
			this.buffer[i] = 0;
			this.styleBuffer[i] = new term.style();
		}
	}
};
