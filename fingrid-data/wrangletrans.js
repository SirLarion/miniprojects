const fs = require("node:fs");
const data = require("./trans.json");

let k = "Norja";

let out = { [k]: [] };

data
  .filter((d) => d["VariableId"] === 57)[0]
  ["Values"].forEach((d) => {
    const t = d["Timestamp"].split("T")[1];
    if (t.startsWith("00") || t.startsWith("12")) {
      out[k].push({
        value: d["Value"],
        t: d["Timestamp"]
          .replace("T00:00", "T00:30")
          .replace("T12:00", "T12:30"),
      });
    }
  });

fs.writeFile("babutrans.json", JSON.stringify(out), () => {});
