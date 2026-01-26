#include "GI_Dialog.h"

#define SCREEN_PADDING 25
#define BOX_HEIGHT 60
#define NAME_HEIGHT 30
#define NAME_CHAR_WIDTH 15
#define BOX_PADDING 5
#define NAME_PADDING 5

GI_Dialog::GI_Dialog(int dialogID) {
    name_ = "???";
}

GI_Dialog::GI_Dialog(std::string name, std::string text) {
    name_ = name;
    text_ = text;
}

GI_Dialog::~GI_Dialog() {

}

void GI_Dialog::handleInput() {
    joypad_buttons_t btn = joypad_get_buttons_pressed(JOYPAD_PORT_1);

    if(btn.a) {
        if(currChars >= text_.length()) {
            timeToDestroy = true;
            return;
        }
        text_ = text_.substr(currChars);
    }
}

void GI_Dialog::update() {

}

void GI_Dialog::renderT3d() {

}

void GI_Dialog::renderRdpq() {
    rdpq_sync_pipe();
    rdpq_set_mode_standard();
    rdpq_mode_combiner(RDPQ_COMBINER_FLAT);
    rdpq_mode_blender(RDPQ_BLENDER_MULTIPLY);
    rdpq_set_prim_color(RGBA32(0, 0, 0, 128));

    rdpq_fill_rectangle(SCREEN_PADDING+10, display_get_height()-SCREEN_PADDING-BOX_HEIGHT-NAME_HEIGHT, SCREEN_PADDING+name_.length()*NAME_CHAR_WIDTH, display_get_height()-SCREEN_PADDING-BOX_HEIGHT);

    rdpq_fill_rectangle(SCREEN_PADDING, display_get_height()-SCREEN_PADDING-BOX_HEIGHT, display_get_width()-SCREEN_PADDING, display_get_height()-SCREEN_PADDING);

    rdpq_text_printf(&(rdpq_textparms_t) {
        .style_id=FONTSTYLE_RED,
    }, FONT_OWREKYNGE_20, SCREEN_PADDING+20, display_get_height()-SCREEN_PADDING-BOX_HEIGHT-NAME_HEIGHT/2.0f+NAME_PADDING, name_.c_str());

    int nbytes = text_.length();

    //debugf("nbytes before paragraph: %d\n", nbytes);

    rdpq_paragraph_t *par = rdpq_paragraph_build(&(rdpq_textparms_t){
            .width = display_get_width()-(2*(SCREEN_PADDING+BOX_PADDING)),
            .height = BOX_HEIGHT-(2*BOX_PADDING),
            .align = ALIGN_LEFT,
            .valign = VALIGN_TOP,
            .wrap = WRAP_WORD,
        }, FONT_PIACEVOLI_16, text_.c_str(), &nbytes);
    
    //debugf("nbytes after paragraph: %d\n", nbytes);
    //debugf("nchars after paragraph: %d\n", par->nchars);

    //currChars = par->nchars;
    currChars = nbytes;

    rdpq_paragraph_render(par, SCREEN_PADDING+2*BOX_PADDING, display_get_height()-SCREEN_PADDING-BOX_HEIGHT+BOX_PADDING);
    rdpq_paragraph_free(par);
}