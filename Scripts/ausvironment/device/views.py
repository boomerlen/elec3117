from django.shortcuts import render
from django.http import HttpResponse

from .models import Device, SoilMoisture

# Create your views here.
def index(request):
    last_10_soil_moisture = SoilMoisture.objects.order_by('-created_at')[:10]
    output = ', '.join([str(soil_moisture.soil_moisture) for soil_moisture in last_10_soil_moisture])
    header = "Soil Moisture: "
    if output == '':
        return HttpResponse("No data available")
    else:
        return HttpResponse(header + output)

def soil_moisture_by_id(request, device_id):
    # Get all soil moistures device with device id device_id
    moisture = SoilMoisture.objects.filter(device__device_id=device_id)
    output = ', '.join([str(soil_moisture.soil_moisture) for soil_moisture in moisture])
    header = f"Soil Moisture of device {device_id}: "
    return HttpResponse(header + output)

def handle_data(request):
    # Get data from request
    try:
        device_id = request.POST['device_id']
    except KeyError:
        return HttpResponse("No device id provided")
    
    try:
        soil_moisture = request.POST['soil_moisture']
    except KeyError:
        return HttpResponse("No soil moisture provided")
    
    # Get device with device id device_id
    try:
        device = Device.objects.get(device_id=device_id)
    except Device.DoesNotExist:
        device = Device(device_id=device_id, device_name=device_id, client_id=device_id)
        device.save()
    
    s = SoilMoisture(device=device, soil_moisture=soil_moisture)
    s.save()
    return HttpResponse("Data received")