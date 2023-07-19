from django.db import models

# Create your models here.
class SoilMoisture(models.Model):
    device = models.ForeignKey('Device', on_delete=models.CASCADE)
    soil_moisture = models.IntegerField()
    created_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.device_id

class Device(models.Model):
    device_id = models.CharField(max_length=100)
    device_name = models.CharField(max_length=100)
    client_id = models.CharField(max_length=100)
    created_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return self.device_id