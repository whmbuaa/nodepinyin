
"use strict";

let timeBefore = new Date().getTime();
var nodepinyin = require('./index');
console.log(`require consume :${new Date().getTime() - timeBefore}`);


console.log(JSON.stringify(nodepinyin.getPinyin("a我退送"))); // 'world'
console.log(`require consume :${new Date().getTime() - timeBefore}`);

console.log("sentence: 我a的家在东北送花奖赏，哪里有森林每款个，还有那满山遍野的打斗高亮。"); // 'world'
console.log(nodepinyin.getPinyin("我a的家在东北送花奖赏，哪里有森林每款个，还有那满山遍野的打斗高亮。")); // 'world'

console.log(`require consume :${new Date().getTime() - timeBefore}`);

