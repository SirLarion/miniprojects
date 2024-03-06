<template>
  <div class="note">
    <div class="note-title">
      <el-input
        id="titlefield"
        :placeholder="currentDate"
        v-model="titleField"
      >
      </el-input>
    </div>
    <div class="note-content">
      <div class="content-scroll-wrapper">
        <el-input
          id="contentfield"
          type="textarea"
          :autosize="{minRows: 17}"
          placeholder="Note content"
          v-model="contentField"
        >
        </el-input>
      </div>
    </div>
    <div class="note-others">
      <div class="colorcode-picker">
        <div 
          class="colorcode-wrapper"
          v-for="(color, index) in colorCodes" 
        >
          <div 
            :class="checkSelectedColor(index)"
            :style="{backgroundColor: color}"
            @click.prevent="selectColor(index)"
          >
          </div>
        </div>
      </div>
      <el-button 
        v-if="contentField" 
        class="save-note"
        @click.prevent="saveNote()"
      >Save</el-button>
      <el-button v-else disabled class="save-disabled">Save</el-button>
    </div>
  </div>
</template>

<script>

export default {
  computed: {
    currentDate () {
      return this.$store.getters.getDate;
    },
    titleField: {
      get() {
        return this.$store.state.notebook.title;
      },
      set(value) {
        this.$store.commit('setTitle', value);
      }
    },
    contentField: {
      get() {
        return this.$store.state.notebook.content;
      },
      set(value) {
        this.$store.commit('setContent', value);
      }
    },
    selectedColorIndex () {
      return this.$store.state.notebook.selectedColorIndex;
    },
    colorCodes () {
      return this.$store.state.notebook.colorCodes;
    },
  },
  methods: {
    saveNote() {
      this.$store.dispatch('saveNote');
    },
    selectColor(index) {
      if(this.selectedColorIndex === index) { // deselect color if already selected
        this.$store.commit('setSelectedColorIndex', null);
      }
      else {
        this.$store.commit('setSelectedColorIndex', index);
      };
    },
    checkSelectedColor(index) {
      if(this.selectedColorIndex === index){
        return 'colorcode-selected';
      }
      else {
        return 'colorcode';
      };
    }
  },
};

</script>

<style>

#titlefield, #contentfield {
  margin-top: 0.3vh !important;
  border-style: none !important;
  border-radius: 0 !important;
}

#titlefield {
  padding: 3vh !important;
  font-size: 2em !important;
}

#contentfield {
  padding: 1.5vh !important;
}

.note {
  padding-top: 2vh;
  width: 87%;
  margin: auto;
}

.note-title {
  margin: 1vh;
  box-sizing: border-box;
  background-color: var(--bg-note);
}

.note-content {
  margin: 1vh;
  margin-top: 2vh;
  display: flex;
  justify-content: center;
  box-sizing: border-box;
  height: 24em;
  background-color: var(--bg-note);
}

.content-scroll-wrapper {
  width: 96%;
  overflow-y: scroll;
  overflow-x: hidden;
}

.note-others {
  display: flex;
  justify-content: space-around;
  align-items: center;
  height: 6vh;
  width: 80vw;
  position: absolute;
  bottom: 20%;
  right: 10vw;
}

.save-note, .save-disabled{
  font-size: 2vw;
  border: none !important;
  border-radius: 1vw !important;
  width: 15vw !important;
  height: 8vw !important;
}

.save-note {
  background-color: #6AA84F !important;
  color: var(--fg-light) !important;
}

.save-disabled {
  background-color: #97aa97 !important;
  color: lightgray !important;
}

.colorcode-picker {
  display: flex;
  justify-content: space-between;
  align-items: center;
  height: 100%; 
  width: 63%;
}

.colorcode, .colorcode-selected {
  height: 90%;
  width: 90%;
  border-radius: 1vw;
}

.colorcode-selected {
  border: solid 2px ghostwhite;
  outline: solid 1px var(--bg-header);
}

.colorcode-wrapper {
  display: flex;
  justify-content: center;
  align-items: center;
  height: 8vw;
  width: 8vw;
  background-color: var(--bg-main); 
}

.colorcode:hover, .colorcode-selected:hover {
  cursor: pointer;
}
</style>
