const state = {
  title: '',
  content: '',
  selectedColorIndex: null,
  colorCodes: ['#CD5C5C', '#32CD32', '#87CEFA', '#FFD700', '#9932CC'],
};

const actions = {
 
  saveNote ({dispatch}) {
    console.log('Saving note...');
    if(!state.title){ //if there's no title set the current date as the title
      state.title = getters.getDate();
    };
    const note = {
        title: state.title,
        content: state.content,
        colorcode: state.colorCodes[state.selectedColorIndex]
    };
    dispatch('createNote', note);
  },

  resetNote () {
    state.title = '';
    state.content = '';
    state.selectedColorIndex = null;
  },
};

const mutations = {

  setTitle (state, payload) {
    //console.log('changing title...', title);
    state.title = payload;
  },
 
  setContent (state, payload) {
    //console.log('changing content...', content);
    state.content = payload;
  }, 

  setSelectedColorIndex (state, payload) {
    state.selectedColorIndex = payload;
  },

};

const getters = {
  getDate: () => {
    return (new Date()).toDateString();
  },
};

export default {
  state,
  actions,
  mutations,
  getters
};
