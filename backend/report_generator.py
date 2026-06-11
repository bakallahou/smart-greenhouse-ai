from reportlab.platypus import (

    SimpleDocTemplate,

    Paragraph,

    Spacer

)

from reportlab.lib.styles import getSampleStyleSheet

from reportlab.lib.pagesizes import letter

from datetime import datetime


def generate_pdf_report(

    disease,

    recommendations,

    alerts,

    sensor_data

):

    pdf = SimpleDocTemplate(

        "static/plant_report.pdf",

        pagesize=letter

    )

    styles = getSampleStyleSheet()

    elements = []

    title = Paragraph(

        "Smart Greenhouse AI Report",

        styles['Title']

    )

    elements.append(title)

    elements.append(Spacer(1,20))


    date_text = Paragraph(

        f"Date: {datetime.now()}",

        styles['BodyText']

    )

    elements.append(date_text)

    elements.append(Spacer(1,20))


    disease_text = Paragraph(

        f"Disease Detection: {disease}",

        styles['BodyText']

    )

    elements.append(disease_text)

    elements.append(Spacer(1,20))


    sensor_text = Paragraph(

        f"""

        Temperature: {sensor_data['temperature']}°C<br/>

        Humidity: {sensor_data['humidity']}%<br/>

        Soil Moisture: {sensor_data['soil']}%<br/>

        Light: {sensor_data['light']}

        """,

        styles['BodyText']

    )

    elements.append(sensor_text)

    elements.append(Spacer(1,20))


    rec_title = Paragraph(

        "AI Recommendations",

        styles['Heading2']

    )

    elements.append(rec_title)

    elements.append(Spacer(1,10))


    for rec in recommendations:

        rec_text = Paragraph(

            f"• {rec}",

            styles['BodyText']

        )

        elements.append(rec_text)


    elements.append(Spacer(1,20))


    alert_title = Paragraph(

        "AI Alerts",

        styles['Heading2']

    )

    elements.append(alert_title)

    elements.append(Spacer(1,10))


    for alert in alerts:

        alert_text = Paragraph(

            f"• {alert}",

            styles['BodyText']

        )

        elements.append(alert_text)


    pdf.build(elements)