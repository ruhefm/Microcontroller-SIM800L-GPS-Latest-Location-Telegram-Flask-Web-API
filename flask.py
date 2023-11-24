# coding=utf-8
import telebot
import flask
import time
token = 'bot_token'
bot = telebot.TeleBot(token, threaded=False)

WEBHOOK_HOST = 'teleapigps.pythonanywhere.com'
WEBHOOK_URL_BASE = "https://%s" % (WEBHOOK_HOST)
WEBHOOK_URL_PATH = "/%s/" % (token)

app = flask.Flask(__name__)

@app.route('/')
def koordinat():
    global lat, longitude, alt, bat
    lat = flask.request.args.get("lat")
    longitude = flask.request.args.get("long")
    alt = flask.request.args.get("alt")
    bat = float(flask.request.args.get("bat"))
    bat = 100-bat

    return f"""lat,long,alt,bat:{lat},{longitude},{alt},{bat}
    """

# Process webhook calls
@app.route(WEBHOOK_URL_PATH, methods=['POST'])
def webhook():
    if flask.request.headers.get('content-type') == 'application/json':
        json_string = flask.request.get_data().decode('utf-8')
        update = telebot.types.Update.de_json(json_string)
        bot.process_new_updates([update])
        return ''
    else:
        flask.abort(403)

@bot.message_handler(commands=['start', 'help'])
def handle_start_help(message):
        bot.send_message(message.chat.id, text="Gunakan /status untuk melihat lokasi.")


@bot.message_handler(commands=['status'])
def handle_start_help(message):

        bot.send_message(message.chat.id, text=f"Latitude:{lat}\nLongitude:{longitude}\nAltitude:{alt}\nPersentase Baterai:{bat} % \n\nhttp://maps.google.com/maps?q=loc:{lat},{longitude}")



bot.remove_webhook()
time.sleep(0.1)
bot.set_webhook(url=WEBHOOK_URL_BASE + WEBHOOK_URL_PATH)






