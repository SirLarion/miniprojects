import Vue from 'vue';
import Vuex from 'vuex';
import { MessageBox } from 'element-ui';
import dm5 from 'dmx-api';

Vue.use(Vuex);

const state = {

  object: undefined,        // The selected Topic/Assoc/TopicType/AssocType.
                            // This object is displayed in detail panel. Its ID appears in the browser URL.
                            // Undefined if there is no selection or a multi-selection.
  activeWorkspace: undefined,
  activeTopicmap: undefined,

  writable: undefined,      // True if the current user has WRITE permission for the selected object.

  popupVisible: false,
  noteStatus: 0, // -1: Failure
                 //  0: Waiting
                 //  1: Success

};

const actions = {

  emptyDisplay () {
    // console.log('emptyDisplay');
    state.object = undefined;
  },

  displayPopupAnimation (_, success) {
    if(success === null){
      state.popupVisible = true;
    }
    else {
        if(success === true){
          state.noteStatus = 1;
        }
        else if(success === false){
          state.noteStatus = -1;
        }
        state.popupVisible = true;
        setTimeout(() => {
            state.popupVisible = false;
            setTimeout(() => state.noteStatus = 0, 1000);
        }, 1300);
    };
  },

  submit ({dispatch}, object) {
    object.update().then(object => {
      dispatch('_processDirectives', object.directives);
    });
  },

  createNote ({dispatch}, {title, content, colorcode}) {
    // Note: for value integration to work at least all identity fields must be filled
    const noteType = dm5.typeCache.getTopicType('dmx.notes.note');
    // Create a new topic model with the note topic type and change the value of it's children
    // to the user inputs
    const noteModel = new dm5.Topic(noteType.newTopicModel(title)).fillChildren();
    noteModel.children['dmx.notes.text'].value = content;
    if(colorcode){
        noteModel.children['dmx.notebook.colorcode'].value = colorcode;
    }
    else {
        // Colorcode defaults to "Yellow legalpad"
        noteModel.children['dmx.notebook.colorcode'].value = '#FFFFAF';
    };
    const res = dm5.restClient.createTopic(noteModel);
    dispatch('displayPopupAnimation', null);

    //Artificial delay for testing loading animation
    setTimeout(() => {
        res.then(note => {
          console.log('Created', note);
          const viewProps = {
              'dmx.topicmaps.x': Math.random()*400, 
              'dmx.topicmaps.y': Math.random()*400, 
              'dmx.topicmaps.pinned': false,
              'dmx.topicmaps.visibility': true
          };
          dm5.restClient.addTopicToTopicmap(state.activeTopicmap.id, note.id, viewProps);
          dispatch('resetNote'); 
          dispatch('displayPopupAnimation', true);
        }, fail => {
            console.log('Creating note failed');
            dispatch('displayPopupAnimation', false);
        });
    }, 1000);
  },

  checkInitConditions ({dispatch}, username) {
    const title = 'Notebook';
    if(username){
        dm5.restClient.getTopicsByType('dmx.workspaces.workspace').then(res => {
            const notebookSpace = {name: title, workspace: res.find(space => space.value === title)};
            dispatch('setActiveWorkspace', notebookSpace).then(workspace => { 
                dm5.restClient.getAssignedTopics(workspace.id, 'dmx.topicmaps.topicmap', false, false).then(maps => {
                    console.log(maps);
                    const notebookMap = {name: title, topicmap: maps.find(map => map.value === title), wsID: workspace.id};
                    const untitledMap = maps.find(map => map.value === 'untitled');
                    dispatch('setActiveTopicmap', notebookMap);
                    if(untitledMap){
                        dm5.restClient.deleteTopic(untitledMap.id).then(res => {
                            console.log('Removed untitled topicmap');
                        }); 
                    };
                });
            });
            
        });
    };
  },

  setActiveWorkspace (_, {name, workspace}) {
    return new Promise((resolve, reject) => {
        if(!workspace){
            dm5.restClient.createWorkspace(name, null, 'dmx.workspaces.private').then(space => {
                console.log(`Initialized ${name} workspace`);
                state.activeWorkspace = space;
                resolve(space);
            });
        }
        else {
            state.activeWorkspace = workspace;
            resolve(workspace);
        };
    });
  },

  setActiveTopicmap (_, {name, topicmap, wsID}) {
    if(!topicmap){
        dm5.restClient.createTopicmap(name, 'dmx.topicmaps.topicmap', {}).then(map => {
            console.log(`Initialized ${name} topicmap`);
            state.activeTopicmap = map;
            dm5.restClient.assignToWorkspace(map.id, wsID);
        });
    }
    else {
        state.activeTopicmap = topicmap;
    };
  },

  loggedIn () {
    initWritable();
  },

  loggedOut () {
    initWritable();
  },

  // WebSocket messages

  _processDirectives (_, directives) {
    console.log(`Mobile: processing ${directives.length} directives`, directives);
    directives.forEach(dir => {
      switch (dir.type) {
      case "UPDATE_TOPIC":
        displayObjectIf(new dm5.Topic(dir.arg));
        break;
      case "DELETE_TOPIC":
        unselectIf(dir.arg.id);
        break;
      case "UPDATE_ASSOCIATION":
        displayObjectIf(new dm5.Assoc(dir.arg));
        break;
      case "DELETE_ASSOCIATION":
        unselectIf(dir.arg.id);
        break;
      };
    });
  }
};

const getters = {

  // Recalculate "object" once the underlying type changes.
  // The detail panel updates when a type is renamed.
  object: state => {
    // console.log('object getter', state.object, state.object && state.typeCache.topicTypes[state.object.uri])
    // ### FIXME: the asCompDef() approach does not work at the moment. Editing an comp def would send an
    // update model with by-URI players while the server expects by-ID players.
    return state.object && (state.object.isType()    ? state.object.asType() :
                            state.object.isCompDef() ? state.object.asCompDef() :
                            state.object)
    // logical copy in createDetail()/updateDetail() (topicmap-model.js of dm5-cytoscape-renderer module)
  }
}

const store = new Vuex.Store({
  state,
  actions,
  getters
});

export default store;


// copy in cytoscape-view.js (module dm5-cytoscape-renderer)
// TODO: unify selection models (see selection.js in dmx-topicmaps module)
function size (idLists) {
  return idLists.topicIds.length + idLists.assocIds.length
}

//

function initWritable () {
   state.object && _initWritable()
}

function _initWritable () {
  state.object.isWritable().then(writable => {
    state.writable = writable
  })
}


