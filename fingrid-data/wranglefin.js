const data = require("./babu.json");
const fs = require("node:fs");

let csv = "";

Object.keys(data).forEach((k) => (csv += `${k},`));

csv += "Timestamp;\n";

let rows = {};

data["Consumption"].forEach(({ t }) => (rows[t] = ""));

Object.keys(data).forEach((k) => {
  data[k].forEach(({ value, t }) => {
    rows[t] += `${value},`;
  });
});

console.log(rows);

Object.keys(rows).forEach((t) => {
  csv += `${rows[t]}"${t}";\n`;
});

fs.writeFile("fin.csv", csv, () => {});
