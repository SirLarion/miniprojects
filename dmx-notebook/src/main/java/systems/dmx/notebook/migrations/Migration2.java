package systems.dmx.notebook.migrations;

import systems.dmx.core.TopicType;
import systems.dmx.core.service.Migration;

public class Migration2 extends Migration {

    @Override
    public void run() {
        TopicType type = dmx.getTopicType("dmx.notes.note");
        type.addCompDef(mf.newCompDefModel(
            "dmx.notes.note", "dmx.notebook.colorcode", "dmx.core.one")
        );
    }
}
