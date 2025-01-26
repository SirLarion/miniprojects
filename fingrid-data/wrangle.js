const data = require("./prod.json");
const fs = require("node:fs");

// let res = [];

let filtered = {};

const OPTIONS = ["00", "12"];

let k = "ElectricityPriceInFinland";

filtered[k] = [];
data[k].forEach((dp) => {
  const t = dp.startTime.split("T")[1];
  if (OPTIONS.some((start) => t.startsWith(start))) {
    filtered[k].push({ value: dp.value, t: dp.startTime });
  }
});

fs.writeFile("babuPrice.json", JSON.stringify(filtered), () => {});
