struct Settings {

  double tempo;
  int quantum;
  int ppqn_index;
  static const std::vector<int> ppqn_options;
  int delay_compensation;
  bool start_stop_sync;

  // Defaults
  Settings() : tempo(120.0), quantum(4), ppqn_index(0), delay_compensation(0), start_stop_sync(true) {}

  // Load from config file
  static Settings Load();

  // Save to config file
  static void Save(const Settings settings);

  //look up ppqn value in ppqn_options vector
  int getPPQN() const;

};

