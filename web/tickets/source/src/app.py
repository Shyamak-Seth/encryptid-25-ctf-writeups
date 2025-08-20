from flask import Flask, request, render_template, send_file
import io
import random

app = Flask(__name__)
VVIP_COUPON = open('/app/coupon.txt', 'r').read().strip()
FLAG = "encryptid{vViP_aT_7r4v1s_iS_cR4zY}"

@app.route('/', methods=['GET'])
def render_index():
    return render_template('index.html')

@app.route('/book', methods=['GET', 'POST'])
def generate_ticket():
    if request.method == 'POST':
        name = request.form.get('name', 'Anonymous')
        level = request.form.get('level', 'Regular')
        ticket_id = str(random.randint(1, 100000))
        ticket_dict = {
            "id": ticket_id,
            "name": name,
            "level": level
        }
        ticket_data = str(ticket_dict)
        return send_file(
            io.BytesIO(ticket_data.encode()),
            as_attachment=True,
            download_name=f"ticket.tkt",
            mimetype='text/plain'
        )

    return render_template('book.html')

@app.route('/checkin', methods=['GET', 'POST'])
def validate_ticket():
    if request.method == 'POST':
        ticket_file = request.files.get('ticket')
        if ticket_file:
            content = ticket_file.read().decode()
            try:
                BLACKLIST = ['__', 'import', 'VVIP_COUPON', 'globals', 'subprocess', 'open']
                for word in BLACKLIST:
                    if word in content:
                        return f"Hacking attempt detected. Touch some grass >:("

                ticket = eval(content)
                if "coupon" in ticket:
                    if ticket['coupon'] == VVIP_COUPON:
                        return f"""
                        <h1>Welcome, Master.</h1>
                        <p>Please accept this gift of ours: {FLAG}"""
                return """
                <h1>Verified!</h1>
                <p>You are now checked in.</p>"""
            except Exception as e:
                print(e)
                return f"<p>Invalid ticket.</p>"

    return render_template('checkin.html')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
