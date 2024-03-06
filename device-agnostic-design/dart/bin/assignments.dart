class Video {
  String name;
  int duration;

  Video({this.name = "Unknown", this.duration = 0});

  @override
  String toString() {
    return "$name ($duration ${duration == 1 ? "second" : "seconds"})";
  }
}

class Playlist {
  List<Video> videos = [];

  Playlist();

  void add(Video video) {
    videos.add(video);
  }

  bool has(String videoName) {
    final match = videos.indexWhere((Video video) =>
      video.name == videoName);

    return match != -1;
  }

  int duration() =>
    videos.fold(0, (int prev, Video curr) => prev + curr.duration);
}

void main() {
  final vid1 = Video(name: "Beep boop", duration: 156);
  final vid2 = Video(name: "Rickroll", duration: 420);
  final vid3 = Video(name: "Game of Thrones, Season 5 - Episode 7", duration: 1760);
  final vid4 = Video(name: "yeaboi", duration: 1);

  final list = Playlist();
  list.add(vid1);
  list.add(vid2);
  list.add(vid3);
  list.add(vid4);

  print(vid1);
  print(list.has("Rickroll"));
  print(list.duration());

}
