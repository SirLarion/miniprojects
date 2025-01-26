const fs = require("node:fs");
const kuntamap = require("./kunnat.json");

const AREAS = Array.from(new Set(Object.values(kuntamap))).reduce(
  (acc, curr) => {
    acc[curr] = 0;
    return acc;
  },
  {},
);

fs.readFileSync("./tuulivoimalat.csv")
  .toString()
  .split("\n")
  .forEach((row) => {
    const [kunta, p] = row.split(",");
    AREAS[kuntamap[kunta]] += Number(p);
  });

let csv = "Maakunta,Teho\n";

Object.keys(AREAS).forEach((a) => {
  if (AREAS[a] !== NaN) {
    csv += `${a},${AREAS[a]}\n`;
  }
});

fs.writeFile("tuuli-maakunnat.csv", csv, () => {});
