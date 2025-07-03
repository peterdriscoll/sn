import { Component, ElementRef, ViewChild } from '@angular/core';
import { HttpClient } from '@angular/common/http';
import { RerunCommandService } from '../services/rerun-command.service';

@Component({
  selector: 'app-stack',
  templateUrl: './stack.component.html',
  standalone: false
})
export class StackComponent {
  @ViewChild('stackid') stackEl!: ElementRef<HTMLDetailsElement>;

  threadnum: number = 0;
  currentstepcount: number = 0;
  stackstepcount: number = 0;

  maxstackframes: number = 100;
  bufstackframes: number = 10;
  startstackframes: number = 0;
  displayedstackframes: number = 0;
  countframes: number = 0;

  frames: any[] = [];

  constructor(
    private http: HttpClient,
    private rerunService: RerunCommandService
  ) {}

  loadstack(opening: boolean): void {
    const field = this.stackEl?.nativeElement;

    if (this.stackstepcount !== this.currentstepcount) {
      this.stackstepcount = this.currentstepcount;
      this.startstackframes = 0;
      this.displayedstackframes = 0;
    }

    if ((opening && !field?.open) || (!opening && field?.open)) {
      if (this.startstackframes < this.maxstackframes && this.startstackframes < this.countframes) {
        const url = `http://127.0.0.1/stack.json?threadnum=${this.threadnum}&maxstackframes=${this.bufstackframes}&startstackframes=${this.startstackframes}&stepcount=${this.stackstepcount}`;
        this.http.get<any>(url).subscribe(response => {
          if (this.stackstepcount === this.currentstepcount && this.stackstepcount === response.stepcount) {
            const records = response.records || [];

            if (this.startstackframes === 0) {
              this.frames = records;
              this.displayedstackframes = 0;
            } else {
              this.frames.push(...records);
            }

            this.displayedstackframes += records.length;
            this.startstackframes += this.bufstackframes;

            this.countframes = response.countframes ?? this.countframes;

            if (this.startstackframes < this.maxstackframes && this.startstackframes < this.countframes) {
              this.loadstack(false);
            }
          }
        });
      }
    }
  }

  reruntostackpoint(stepcount: number): void {
    this.currentstepcount = stepcount;
    this.rerunService.updateRerunCommand(stepcount);
  }

  trustAsHtml(value: string): string {
    return value;
  }
}
