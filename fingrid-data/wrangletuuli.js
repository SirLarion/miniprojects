const fs = require("node:fs");

let csv = "Päivä,Aika,Kalajoki,Vaasa,Kristiinankaupunki\n";

let msMap = {};

fs.readFileSync("./tuuli-mittaus-kalajoki.csv")
  .toString()
  .split("\n")
  .forEach((row) => {
    const [, v, k, p, h, ms] = row.split(",");
    if (h == "12:00" || h == "00:00") {
      msMap[`${v}-${k}-${p}T${h.split(":")[0]}:30:00`] = `${ms}`;
    }
  });

fs.readFileSync("./tuuli-mittaus-vaasa.csv")
  .toString()
  .split("\n")
  .forEach((row) => {
    const [, v, k, p, h, ms] = row.split(",");
    if (h === "12:00" || h === "00:00") {
      msMap[`${v}-${k}-${p}T${h.split(":")[0]}:30:00`] += `,${ms}`;
    }
  });

fs.readFileSync("./tuuli-mittaus-kristiinankaupunki.csv")
  .toString()
  .split("\n")
  .forEach((row) => {
    const [, v, k, p, h, ms] = row.split(",");
    if (h === "12:00" || h === "00:00") {
      msMap[`${v}-${k}-${p}T${h.split(":")[0]}:30:00`] += `,${ms}`;
    }
  });

Object.keys(msMap).forEach((t) => {
  const [d, h] = t.split("T");
  csv += `${d},${h},${msMap[t]}\n`;
});

fs.writeFile("tuuli-mittaus.csv", csv, () => {});
