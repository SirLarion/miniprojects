import 'package:flutter/material.dart';

class ButtonRow extends StatelessWidget {

  @override
  Widget build(BuildContext ctx) {
    return Row(children: [
      ElevatedButton(onPressed: () => print("hello"), child: const Text("Hello"))
      OutlinedButton(onPressed: () => print("world"), child: const Text("world"))
      TextButton(onPressed: () => print("!"), child: const Text("!"))
    ]);
  }
}

main() {
  runApp(MaterialApp(
      home: Scaffold(
        body: ButtonRow())));
}
